#include "aula_umacro.h"
#include "kbdef.h"
#include "keycodes.h"
#include "matrix.h"
#include "report.h"
#include "tick.h"
#include <stdint.h>
#include <stdbool.h>

/*
 * マクロの再生（AULA F75J キー配置エディタ用）。
 *
 * マクロの表は CODE の決まった場所（layouts/default/layout.c）にあり、エディタが
 * 書き込むときに差し込む。
 *
 *   umacro_dir[16][2]     = { 手順の開始位置, 繰り返し方 }        0xE690（64 バイト）
 *   umacro_steps[N][2]    = { 種類, 値 }                          0xE6D0 から
 *     種類: 0 = 終わり, 1 = 叩く, 2 = 押す, 3 = 離す, 4 = 待つ（値 = ms）
 *   繰り返し方: 0 = 1 回だけ, 1 = 押している間くり返す, 2 = もう一度押すまでくり返す
 *
 * 再生はふだんのループ（kb_update）で行い、時間は tick_ms() で測る。キーの「押す」と
 * 「離す」の間には必ず少し待つ。続けて送ると、前の報告が USB に出る前に上書きされて、
 * パソコンに届かないことがあるため。
 */
#define STEP_END     0
#define STEP_TAP     1
#define STEP_PRESS   2
#define STEP_RELEASE 3
#define STEP_DELAY   4

#define MODE_ONCE   0
#define MODE_HOLD   1
#define MODE_TOGGLE 2

#define GAP_MS 12 /* 押す・離すの間の待ち（USB の報告が 1 回は出るように） */

extern const __code uint16_t umacro_dir[UMACRO_COUNT][2];
extern const __code uint16_t umacro_steps[][2];

static __xdata uint8_t  um_active;   /* 再生中のマクロ番号 + 1（0 = 止まっている） */
static __xdata uint16_t um_pos;      /* 次の手順 */
static __xdata uint16_t um_wait_from;
static __xdata uint16_t um_wait_ms;
static __xdata uint16_t um_tap_code; /* 叩いている途中のキー（離す待ち） */
static __xdata bool     um_held;     /* マクロのキーを押したままか */
static __xdata bool     um_stop_req; /* 次のくり返しの前で止める */

static void um_stop(void)
{
    if (um_tap_code) {
        matrix_virtual_key(um_tap_code, false);
        um_tap_code = 0;
    }
    um_active = 0;
}

static void um_wait(uint16_t ms)
{
    um_wait_from = tick_ms();
    um_wait_ms   = ms;
}

bool aula_umacro_process(uint16_t keycode, bool pressed)
{
    if (keycode < MACRO_1 || keycode > MACRO_16) {
        return true;
    }
    const uint8_t idx  = (uint8_t)(keycode - MACRO_1);
    const uint8_t mode = (uint8_t)umacro_dir[idx][1];

    if (pressed) {
        if (um_active == idx + 1 && mode == MODE_TOGGLE) {
            um_stop_req = true; /* もう一度押した: 今の 1 回が終わったら止める */
            return false;
        }
        if (um_active) {
            um_stop(); /* ほかのマクロが動いていたら止める */
        }
        um_active   = (uint8_t)(idx + 1);
        um_pos      = umacro_dir[idx][0];
        um_held     = true;
        um_stop_req = false;
        um_wait(0);
    } else if (um_active == idx + 1) {
        um_held = false;
        if (mode == MODE_HOLD) {
            um_stop_req = true;
        }
    }
    return false;
}

void aula_umacro_task(void)
{
    if (!um_active) {
        return;
    }
    if ((uint16_t)(tick_ms() - um_wait_from) < um_wait_ms) {
        return;
    }
    if (um_tap_code) { /* 叩く: 押したあと、少し待ってから離す */
        matrix_virtual_key(um_tap_code, false);
        um_tap_code = 0;
        um_wait(GAP_MS);
        return;
    }

    const uint8_t  idx  = (uint8_t)(um_active - 1);
    const uint8_t  type = (uint8_t)umacro_steps[um_pos][0];
    const uint16_t val  = umacro_steps[um_pos][1];

    switch (type) {
        case STEP_TAP:
            matrix_virtual_key(val, true);
            um_tap_code = val;
            um_pos++;
            um_wait(GAP_MS);
            return;
        case STEP_PRESS:
            matrix_virtual_key(val, true);
            um_pos++;
            um_wait(GAP_MS);
            return;
        case STEP_RELEASE:
            matrix_virtual_key(val, false);
            um_pos++;
            um_wait(GAP_MS);
            return;
        case STEP_DELAY:
            um_pos++;
            um_wait(val);
            return;
        default: { /* 終わり: くり返すか止めるか */
            const uint8_t mode = (uint8_t)umacro_dir[idx][1];
            const bool    again = (mode == MODE_HOLD && um_held && !um_stop_req) ||
                               (mode == MODE_TOGGLE && !um_stop_req);
            if (again) {
                um_pos = umacro_dir[idx][0];
                um_wait(GAP_MS);
            } else {
                um_stop();
            }
            return;
        }
    }
}
