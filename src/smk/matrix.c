#include "matrix.h"
#include "report.h"
#include "debug.h"
#include "layout.h"
#include "user_layout.h"
#include "kb.h"
#include "user_matrix.h"
#include "kbdef.h"
#include "host.h"
#include "delay.h"
#include "indicators.h"
#include "sleep.h"
#include <stdlib.h>
#include <stdbool.h>

typedef uint8_t matrix_col_t;

matrix_col_t matrix[MATRIX_COLS];
matrix_col_t matrix_previous[MATRIX_COLS];

volatile bool matrix_updated;

uint8_t action_layer;

// Projet AULA F75J : keycode envoyé à l'appui, par position, pour relâcher le même.
static __xdata uint16_t pressed_code[MATRIX_ROWS][MATRIX_COLS];

uint8_t default_layer;

void matrix_init()
{
    action_layer   = 0;
    default_layer  = 0;
    matrix_updated = false;

    for (int i = 0; i < MATRIX_COLS; i++) {
        matrix[i]          = 0;
        matrix_previous[i] = 0;
        for (uint8_t r = 0; r < MATRIX_ROWS; r++) {
            pressed_code[r][i] = KC_NO; // la XDATA n'est pas remise à zéro au démarrage
        }
    }
}

void set_default_layer(uint8_t layer)
{
    default_layer = layer;
}

static uint16_t resolve_keycode(uint16_t base, uint8_t row, uint8_t col)
{
    if (!action_layer) {
        return base;
    }

    const uint16_t overlay = keymaps[action_layer][row][col];
    return (overlay == KC_TRANSPARENT) ? base : overlay;
}

static void send_keycode(uint16_t qcode, bool pressed)
{
    if (IS_MODIFIER_KEYCODE(qcode)) {
        if (pressed) {
            add_mods(MOD_BIT((uint8_t)(qcode & 0xFF)));
        } else {
            del_mods(MOD_BIT((uint8_t)(qcode & 0xFF)));
        }
        send_keyboard_report();
        return;
    }

    if (IS_BASIC_KEYCODE(qcode)) {
        if (pressed) {
            add_key((uint8_t)(qcode & 0xFF));
        } else {
            del_key((uint8_t)(qcode & 0xFF));
        }
        send_keyboard_report();
        return;
    }

    /*
     * Combinaisons modificateur + touche (LCTL(KC_C), LALT(KC_TAB), ...).
     * Ajout du projet AULA F75J. Les 5 bits de QK_MODS : bits 0-3 = Ctrl,
     * Shift, Alt, GUI ; bit 4 = côté droit. Au relâchement, les modificateurs
     * de la combinaison sont retirés même si l'utilisateur tient le même
     * modificateur physiquement -- limite assumée de cette première version.
     */
    if (IS_QK_MODS(qcode)) {
        const uint8_t m5   = (uint8_t)QK_MODS_GET_MODS(qcode);
        const uint8_t mask = (m5 & 0x10) ? (uint8_t)((m5 & 0x0F) << 4) : (uint8_t)(m5 & 0x0F);
        const uint8_t key  = (uint8_t)QK_MODS_GET_BASIC_KEYCODE(qcode);

        if (pressed) {
            add_mods(mask);
            if (key) {
                add_key(key);
            }
        } else {
            if (key) {
                del_key(key);
            }
            del_mods(mask);
        }
        send_keyboard_report();
        return;
    }

    if (IS_SYSTEM_KEYCODE(qcode)) {
        host_system_send(pressed ? keycode_to_system(qcode) : 0);
        return;
    }

    if (IS_CONSUMER_KEYCODE(qcode)) {
        host_consumer_send(pressed ? keycode_to_consumer(qcode) : 0);
        return;
    }
}

#ifdef TAP_HOLD_ENABLE
/*
 * TAP / HOLD (projet AULA F75J) : MT(mods, kc) et LT(couche, kc).
 *
 *   - relâchée avant TAPPING_TERM, sans autre appui entre-temps -> TAP : kc
 *   - tenue plus de TAPPING_TERM                                   -> HOLD
 *   - une autre touche enfoncée pendant l'attente                  -> HOLD
 *     (« hold on other key press » : Ctrl+C reste fiable même tapé vite)
 *
 * Les touches en cours sont repérées par leur POSITION, pas par leur keycode :
 * un LT change la couche, donc relire le keycode au relâchement serait faux.
 */
#    ifndef TAPPING_TERM
#        define TAPPING_TERM 200
#    endif
#    define TH_SLOTS 4

#    include "tick.h"

typedef struct {
    uint8_t  state; // 0 libre, 1 en attente, 2 tenue
    uint8_t  row, col;
    uint16_t code;
    uint16_t t0;
} tap_hold_t;

static __xdata tap_hold_t th[TH_SLOTS];

static uint8_t th_mod_mask(uint16_t code)
{
    const uint8_t m5 = (uint8_t)((code >> 8) & 0x1F);
    return (m5 & 0x10) ? (uint8_t)((m5 & 0x0F) << 4) : (uint8_t)(m5 & 0x0F);
}

static void th_hold_on(__xdata tap_hold_t *s)
{
    s->state = 2;
    if (IS_QK_MOD_TAP(s->code)) {
        add_mods(th_mod_mask(s->code));
        send_keyboard_report();
    } else {
        // relatif à la couche de base courante (profils : base = 2 x profil)
        action_layer = (uint8_t)(default_layer + ((s->code >> 8) & 0x0F));
    }
}

static void th_hold_off(__xdata tap_hold_t *s)
{
    if (IS_QK_MOD_TAP(s->code)) {
        del_mods(th_mod_mask(s->code));
        send_keyboard_report();
    } else {
        clear_keys();
        action_layer = 0;
    }
}

static void th_resolve_pending_as_hold(void)
{
    for (uint8_t i = 0; i < TH_SLOTS; i++) {
        if (th[i].state == 1) {
            th_hold_on(&th[i]);
        }
    }
}

// Appelé à chaque passage de la boucle principale : bascule en HOLD les attentes échues.
static void th_task(void)
{
    const uint16_t now = tick_ms();
    for (uint8_t i = 0; i < TH_SLOTS; i++) {
        if (th[i].state == 1 && (uint16_t)(now - th[i].t0) >= TAPPING_TERM) {
            th_hold_on(&th[i]);
        }
    }
}

// true si l'évènement a été consommé par le tap/hold.
static bool th_process(uint8_t row, uint8_t col, uint16_t base, bool pressed)
{
    if (!pressed) {
        for (uint8_t i = 0; i < TH_SLOTS; i++) {
            if (th[i].state && th[i].row == row && th[i].col == col) {
                if (th[i].state == 1) { // TAP
                    const uint8_t kc = (uint8_t)(th[i].code & 0xFF);
                    th[i].state      = 0;
                    if (kc) {
                        add_key(kc);
                        send_keyboard_report();
                        del_key(kc);
                        send_keyboard_report();
                    }
                } else {
                    th[i].state = 0;
                    th_hold_off(&th[i]);
                }
                return true;
            }
        }
        return false;
    }

    // Tout appui tranche d'abord les attentes en cours en faveur du HOLD.
    th_resolve_pending_as_hold();

    const uint16_t code = resolve_keycode(base, row, col);
    if (!IS_QK_MOD_TAP(code) && !IS_QK_LAYER_TAP(code)) {
        return false;
    }
    for (uint8_t i = 0; i < TH_SLOTS; i++) {
        if (!th[i].state) {
            th[i].state = 1;
            th[i].row   = row;
            th[i].col   = col;
            th[i].code  = code;
            th[i].t0    = tick_ms();
            return true;
        }
    }
    return true; // plus d'emplacement libre : on ignore l'appui plutôt que d'envoyer un code invalide
}
#endif

static void process_key_state(uint8_t row, uint8_t col, bool pressed)
{
    const uint16_t base = keymaps[default_layer][row][col];

#ifdef TAP_HOLD_ENABLE
    if (th_process(row, col, base, pressed)) {
        return;
    }
#endif

    if (IS_QK_MOMENTARY(base)) {
        if (pressed) {
            // relatif à la couche de base courante (profils : base = 2 x profil)
            action_layer = (uint8_t)(default_layer + QK_MOMENTARY_GET_LAYER(base));
        } else {
            clear_keys();
            action_layer = 0;
        }
        return;
    }

    /*
     * Projet AULA F75J : relâcher ce qui a été ENFONCÉ. Relire le keycode au
     * relâchement donnait un autre code si la couche avait changé entre-temps
     * (Fn lâché avant la touche) : une combinaison Win+E posée en couche Fn
     * laissait Win enfoncé, puisque `clear_keys()` ne retire pas les
     * modificateurs. On mémorise donc le code envoyé à l'appui, par position.
     */
    uint16_t qcode;
    if (pressed) {
        qcode                   = resolve_keycode(base, row, col);
        pressed_code[row][col] = qcode;
    } else {
        qcode = pressed_code[row][col];
        if (qcode == KC_NO) {
            qcode = resolve_keycode(base, row, col); // appui jamais vu (démarrage)
        }
        pressed_code[row][col] = KC_NO;
    }
    matrix_virtual_key(qcode, pressed);
}

/*
 * Envoie un keycode comme si une touche était enfoncée / relâchée (projet AULA
 * F75J : molette, et touches de la matrice). Passe par les mêmes crochets.
 */
void matrix_virtual_key(uint16_t qcode, bool pressed)
{
    if (!kb_process_record(qcode, pressed)) {
        return;
    }

    if (!layout_process_record(qcode, pressed)) {
        return;
    }

    send_keycode(qcode, pressed);
}

// Couche effectivement active (la superposition si elle existe, sinon la base).
uint8_t matrix_active_layer(void)
{
    return action_layer ? action_layer : default_layer;
}

uint8_t matrix_base_layer(void)
{
    return default_layer;
}

/*
 * Profils (projet AULA F75J) : profil p = couches 2p (base) et 2p+1 (Fn).
 * Relâche tout avant de changer, sinon une touche tenue resterait collée.
 */
void matrix_set_profile(uint8_t profile)
{
    clear_keys();
    action_layer  = 0;
    default_layer = (uint8_t)(profile * 2u);
}

void matrix_scan_full(void)
{
    indicators_pwm_disable();

    user_matrix_sinks_off();

    user_matrix_scan_pre();
    user_matrix_cols_deselect_all();

    for (uint8_t col = 0; col < MATRIX_COLS; col++) {
        user_matrix_col_select(col);

        delay_us(10); // let the row lines settle before sampling
        const uint8_t sample1 = user_matrix_read_rows();
        delay_us(10);
        const uint8_t sample2 = user_matrix_read_rows();

        if (sample1 == sample2) {
            matrix[col] = ~sample1;
        }

        user_matrix_col_deselect(col);
    }

    user_matrix_scan_post();

    indicators_pwm_enable();

    matrix_updated = true;
}

uint8_t matrix_task()
{
#ifdef TAP_HOLD_ENABLE
    th_task();
#endif

    if (!matrix_updated) {
        return false;
    }

    // Snapshot the scan-written matrix[], then diff it against
    // matrix_previous[]. No lock needed: each column byte reads atomically, so a
    // concurrent scan lands cleanly on one side of the read - at worst a
    // transition is split across two main-loop iterations, never lost.
    matrix_col_t snapshot[MATRIX_COLS];
    matrix_updated = false;
    for (uint8_t i = 0; i < MATRIX_COLS; i++) {
        snapshot[i] = matrix[i];
    }

    bool matrix_changed = false;

    for (uint8_t col = 0; col < MATRIX_COLS; col++) {
        const matrix_col_t current_col = snapshot[col];
        const matrix_col_t col_changes = current_col ^ matrix_previous[col];
        if (!col_changes) {
            continue;
        }
        matrix_changed = true;
        sleep_note_activity(); // a key changed state; reset the inactivity timer

        matrix_col_t row_mask = 1;
        for (uint8_t row = 0; row < MATRIX_ROWS; row++, row_mask <<= 1) {
            if (col_changes & row_mask) {
                const bool key_pressed = current_col & row_mask;
                process_key_state(row, col, key_pressed);
            }
        }

        matrix_previous[col] = current_col;
    }

    return matrix_changed;
}
