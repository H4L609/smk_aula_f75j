#include "tick.h"
#include "systick.h"
#include "matrix.h"
#include "indicators.h"
#include "keyboard.h"
#include "sleep.h"
#include "kbdef.h"
#include <stdbool.h>
#include <stdint.h>

// subframes must outnumber the scans, or the refresh rate is the scan rate divided by the subframe count.
#ifndef LED_SUBFRAMES_PER_SCAN
#    define LED_SUBFRAMES_PER_SCAN 1
#endif

static volatile bool    scan_due;
static volatile uint8_t subframes_since_scan;

/*
 * Horloge approximative en millisecondes (projet AULA F75J : tap/hold).
 * Chaque tic vaut ~400 us (sous-trame LED ; le balayage de matrice, ~100 us
 * d'armement + ~320 us de balayage, en est proche). Précision ~10 %, largement
 * suffisante pour un TAPPING_TERM. Lire avec tick_ms() depuis la boucle
 * principale.
 */
static volatile uint16_t tick_ms_counter;
static uint8_t           tick_ms_acc; // unités de 100 us

static void tick_advance(void)
{
    tick_ms_acc += 4;
    if (tick_ms_acc >= 10) {
        tick_ms_acc -= 10;
        tick_ms_counter++;
    }
}

uint16_t tick_ms(void)
{
    uint16_t a, b;
    do { // lecture 16 bits non atomique sur 8051 : relire jusqu'à stabilité
        a = tick_ms_counter;
        b = tick_ms_counter;
    } while (a != b);
    return a;
}

void tick_init(void)
{
    scan_due             = true;
    subframes_since_scan = 0;

    systick_init();
}

static void run_matrix_scan(void)
{
    systick_arm(SYSTICK_SLOT_MATRIX_SCAN);
    matrix_scan_full();
}

static void run_led_subframe(void)
{
    systick_arm(SYSTICK_SLOT_LED_SUBFRAME);

    indicators_pre_update();
    const bool frame_wrapped = indicators_update_step(&keyboard_state, 0);
    indicators_post_update();

    sleep_note_frame(frame_wrapped);
}

void tick_dispatch(void)
{
    tick_advance();

    if (scan_due) {
        scan_due             = false;
        subframes_since_scan = 0;
        run_matrix_scan();
        return;
    }

    run_led_subframe();

    if (++subframes_since_scan >= LED_SUBFRAMES_PER_SCAN) {
        scan_due = true;
    }
}

void tick_pause(void)
{
    systick_pause();
}

void tick_resume(void)
{
    systick_resume();
}
