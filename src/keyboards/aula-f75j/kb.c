#include "kbdef.h"
#include "settings.h"
#include "matrix.h"
#include "kb.h"
#include "keyboard.h"
#include "report.h"
#include "usb.h"
#include "tick.h" /* user_matrix_pressed() は kbdef.h で宣言されている */
#include <stdint.h>
#include <stdbool.h>

#include "aula_encoder.h"
#include "aula_umacro.h"
#include "aula_mouse.h"
#include "aula_macro.h"
#include "aula_layout.h"
#ifdef RF_EUART0
#    include "aula_rf.h"
#endif

/*
 * Logique clavier.
 *
 * Le sans-fil passe par l'EUART0 (BK3632), pas par le SPI bit-bangé de l'Air60 :
 * voir `aula_rf.c`. Le sélecteur à glissière est échantillonné par `rf_task()`,
 * qui décide seul du transport ; ici on se contente d'aiguiller les rapports.
 *
 * Pas de switch mode-OS : sa broche n'est pas établie. Restent inconnues P0.0,
 * P0.1, P0.5, P0.6, P4.1, P4.4 et P7.7 — P7.4 et P4.5 sont maintenant
 * identifiées comme le sélecteur de connexion, et P4.7 comme la ligne « module
 * prêt » de la liaison série.
 */

void kb_init(void)
{
#ifdef RF_EUART0
    rf_init();
#endif
}

void kb_update_switches(void)
{
}

extern void indicators_next_effect(void);
extern void indicators_next_color(void);
extern void indicators_toggle_direction(void);
extern void indicators_prev_effect(void);
extern void indicators_brightness_up(void);
extern void indicators_brightness_down(void);
extern void indicators_speed_up(void);
extern void indicators_speed_down(void);
#ifdef RF_EUART0
extern void indicators_toggle_rf_diag(void);
#endif

/*
 * Les keycodes LNK_BT1..3 ne choisissent que l'emplacement Bluetooth : le
 * transport, lui, vient de la glissière. Appuyer sur l'emplacement déjà actif
 * relance l'appairage, comme le raccourci d'usine qui pose le bit 0x2C.5.
 *
 * Les six keycodes RGB étaient posés dans la keymap depuis le début sans que
 * rien ne les traite : ni ce fichier ni `layout_process_record` ne les
 * reconnaissaient, et ils tombaient dans `send_keycode()` où aucun prédicat ne
 * les prend. Ils appellent maintenant le moteur de rendu.
 */
/*
 * AULA F75J : porte de secours vers le bootloader ISP (BOOT_HOLD).
 *
 * Le compteur avance dans `aula_boot_tick()`, appelé depuis le même tic que
 * `aula_macro_tick()` (environ 420 µs). Le saut lui-même se fait depuis la
 * boucle principale (`kb_update`), jamais depuis une ISR : voir le commentaire
 * de `usb_task()` dans src/smk/usb.c.
 */
#define BOOT_HOLD_TICKS ((uint16_t)((3000ul * 1000ul) / 420ul)) /* ~3 s */

static volatile uint16_t boot_hold_ticks;
static volatile __bit    boot_hold_armed;
static volatile __bit    boot_hold_fire;

extern void indicators_pwm_disable(void);
extern void indicators_profile_flash(uint8_t profile); /* layouts/default/indicators.c */
extern void isp_jump(void);

void aula_boot_tick(void)
{
    if (boot_hold_armed && !boot_hold_fire) {
        if (++boot_hold_ticks >= BOOT_HOLD_TICKS) {
            boot_hold_fire = 1;
        }
    }
}

bool kb_process_record(uint16_t keycode, bool key_pressed)
{
    uint8_t slot;

    if (keycode == BOOT_HOLD) {
        boot_hold_ticks = 0;
        boot_hold_armed = key_pressed;
        return false;
    }

    /* AULA F75J : souris (clics, molette, déplacement, double-clic). */
    if (!aula_mouse_process(keycode, key_pressed)) {
        return false;
    }

    /* AULA F75J : macros utilisateur. */
    if (!aula_umacro_process(keycode, key_pressed)) {
        return false;
    }

    /* AULA F75J : changement de profil, persisté. */
    if (keycode >= PROF_1 && keycode <= PROF_NEXT) {
        if (key_pressed) {
            uint8_t p = (keycode == PROF_NEXT)
                            ? (uint8_t)((AULA_PROFILE_SAVE + 1u) % KEYMAP_PROFILES)
                            : (uint8_t)(keycode - PROF_1);
            AULA_PROFILE_SAVE = p;
            matrix_set_profile(p);
            settings_mark_dirty();
            indicators_profile_flash(p);
        }
        return false;
    }

    /* Tant que la frappe automatique n'est pas revenue au repos, elle avale
     * TOUT : c'est ce qui rend muette la touche qui l'arrête, et ce qui empêche
     * le relâchement de la combinaison d'activation de l'arrêter aussitôt. */
    if (aula_macro_intercept(key_pressed)) {
        return false;
    }

    /* Après la frappe automatique, avant le répartiteur : la traduction ne
     * touche que les keycodes 0x04-0x38, donc les keycodes maison passent. */
    if (aula_layout_intercept(keycode, key_pressed)) {
        return false;
    }

    switch (keycode) {
        case FX_NEXT:
            if (key_pressed) indicators_next_effect();
            return false;
        case FX_PREV:
            if (key_pressed) indicators_prev_effect();
            return false;
        case BRI_UP:
            if (key_pressed) indicators_brightness_up();
            return false;
        case BRI_DN:
            if (key_pressed) indicators_brightness_down();
            return false;
        case SPD_UP:
            if (key_pressed) indicators_speed_up();
            return false;
        case SPD_DN:
            if (key_pressed) indicators_speed_down();
            return false;
        case CLR_NEXT:
            if (key_pressed) indicators_next_color();
            return false;
        case DIR_TOG:
            if (key_pressed) indicators_toggle_direction();
            return false;
        case MACRO_TG:
            if (key_pressed) aula_macro_arm();
            return false;
        case LAYOUT_AZ:
            if (key_pressed) aula_layout_toggle();
            return false;
        case EURO:
            if (key_pressed) aula_layout_euro();
            return false;
#ifdef RF_EUART0
        case RF_DIAG:
            if (key_pressed) indicators_toggle_rf_diag();
            return false;
        case RF_PAIR:
            if (key_pressed) rf_request_pairing();
            return false;
#endif

        case LNK_BT1:
            slot = 1;
            break;
        case LNK_BT2:
            slot = 2;
            break;
        case LNK_BT3:
            slot = 3;
            break;
        default:
            return true;
    }

#ifdef RF_EUART0
    if (key_pressed) {
        if (rf_link() == RF_LINK_BT && rf_bt_slot() == slot) {
            rf_request_pairing();
        } else {
            rf_set_bt_slot(slot);
        }
    }
#else
    slot;
    key_pressed;
#endif
    return false;
}

/*
 * AULA F75J : タップと長押しを見分ける時間（ms）。
 * キー表の見出し（0xE000）の 14〜15 バイト目に、エディタが書き込む。
 * 0 や範囲外（前の版で書き込んだものなど）は 200 ms として扱う。
 */
extern const __code uint8_t keymap_header[16];

uint16_t aula_tapping_term(void)
{
    const uint16_t ms = (uint16_t)(keymap_header[14] | ((uint16_t)keymap_header[15] << 8));
    return (ms >= 100u && ms <= 1000u) ? ms : 200u;
}

/*
 * AULA F75J : 非常口。Esc と F12 を一緒に 2 秒押し続けると、プロファイル 1 に戻る。
 *
 * キーの割り当てとは関係なく、配線上の位置（Esc = 行 0・列 0、F12 = 行 0・列 13）を
 * user_matrix_pressed() で直接見る。そのため、どんな配置でも消えない。
 * 一度働いたら、2 つのキーを離すまでは、もう働かない。
 */
#define ESCAPE_HOLD_MS 2000u

static __xdata uint16_t escape_t0;
static __xdata uint8_t  escape_state; /* 0 = 待ち、1 = 数えている、2 = 働いた（離すまで待つ） */

static void escape_task(void)
{
    const bool both = (user_matrix_pressed(0) & 0x01u) && (user_matrix_pressed(13) & 0x01u);

    if (!both) {
        escape_state = 0;
        return;
    }
    if (escape_state == 0) {
        escape_state = 1;
        escape_t0    = tick_ms();
    } else if (escape_state == 1 && (uint16_t)(tick_ms() - escape_t0) >= ESCAPE_HOLD_MS) {
        escape_state      = 2;
        AULA_PROFILE_SAVE = 0;
        matrix_set_profile(0);
        settings_mark_dirty();
        indicators_profile_flash(0);
    }
}

void kb_update(void)
{
    escape_task();

    if (boot_hold_fire) {
        indicators_pwm_disable(); // le bootloader ne connaît pas ces broches
        isp_jump();               // ne revient jamais
    }

    aula_encoder_task();
    aula_umacro_task();
    aula_mouse_task();
    aula_macro_task();
    aula_layout_task();
#ifdef RF_EUART0
    rf_task();
#endif
}

void kb_send_report(__xdata report_keyboard_t *report)
{
#ifdef RF_EUART0
    if (rf_is_wireless()) {
        rf_send_report(report);
        return;
    }
#endif
    usb_send_report(report);
}

void kb_send_nkro(__xdata report_nkro_t *report)
{
#ifdef RF_EUART0
    if (rf_is_wireless()) {
        rf_send_nkro(report);
        return;
    }
#endif
    usb_send_nkro(report);
}

#ifdef MOUSE_ENABLE
void kb_send_mouse(__xdata report_mouse_t *report)
{
#    ifdef RF_EUART0
    if (rf_is_wireless()) {
        rf_send_mouse(report);
        return;
    }
#    endif
    usb_send_mouse(report);
}
#endif

void kb_send_extra(__xdata report_extra_t *report)
{
#ifdef RF_EUART0
    if (rf_is_wireless()) {
        rf_send_extra(report);
        return;
    }
#endif
    usb_send_extra(report);
}
