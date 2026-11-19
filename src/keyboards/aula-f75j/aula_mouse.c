#include "aula_mouse.h"
#include "kbdef.h"
#include "kb.h"
#include "keycodes.h"
#include "report.h"
#include "tick.h"
#include <stdint.h>
#include <stdbool.h>

/*
 * マウス操作（AULA F75J）。
 *
 *   KC_MS_BTN1〜5        左・右・中クリック、戻る、進む（押している間、ボタンを押したまま）
 *   KC_MS_WH_UP/DOWN     上下スクロール（押すと 1 段。押し続けると、くり返す）
 *   KC_MS_WH_LEFT/RIGHT  左右スクロール（同上）
 *   KC_MS_UP/DOWN/LEFT/RIGHT  カーソル移動（押している間。だんだん速くなる）
 *   DBLCLK_L / DBLCLK_R  左 / 右のダブルクリック
 *
 * 報告の形は、出荷時の F75J のマウスと同じ（report.h の report_mouse_t）。
 * 有線は USB、無線は 2.4GHz / Bluetooth の枠で送る（kb.c の kb_send_mouse）。
 * 移動とスクロールは「相対値」なので、送ったら 0 に戻す。
 */
#define WHEEL_DELAY_MS  300 /* 押し続けてから、くり返しが始まるまで */
#define WHEEL_REPEAT_MS 60  /* くり返しの間隔 */
#define MOVE_INTERVAL   16  /* カーソル移動を送る間隔（ms） */
#define MOVE_SPEED_MIN  2   /* 1 回の移動量（はじめ） */
#define MOVE_SPEED_MAX  20  /* 1 回の移動量（いちばん速いとき） */
#define MOVE_ACCEL_MS   800 /* いちばん速くなるまでの時間 */
#define DBL_GAP_MS      30  /* ダブルクリックの押す・離すの間 */

static __xdata report_mouse_t mreport;
static __xdata uint8_t        buttons;
static __xdata uint8_t        wheel_dir;  /* 押しているスクロール: bit0 上, 1 下, 2 左, 3 右 */
static __xdata uint8_t        move_dir;   /* 押しているカーソル移動: bit0 上, 1 下, 2 左, 3 右 */
static __xdata uint16_t       wheel_t0, wheel_next;
static __xdata uint16_t       move_t0, move_next;
static __xdata uint8_t        dbl_step;   /* 0 = なし。1〜4 = ダブルクリックの途中 */
static __xdata uint8_t        dbl_btn;
static __xdata uint16_t       dbl_next;

static void send(int16_t x, int16_t y, int8_t wheel, int8_t pan)
{
    mreport.report_id = REPORT_ID_MOUSE;
    mreport.buttons   = buttons;
    mreport.x         = x;
    mreport.y         = y;
    mreport.wheel     = wheel;
    mreport.pan       = pan;
    kb_send_mouse(&mreport);
}

static void wheel_once(void)
{
    const int8_t v = (wheel_dir & 1) ? 1 : (wheel_dir & 2) ? -1 : 0;
    const int8_t h = (wheel_dir & 8) ? 1 : (wheel_dir & 4) ? -1 : 0;
    if (v || h) {
        send(0, 0, v, h);
        send(0, 0, 0, 0);
    }
}

bool aula_mouse_process(uint16_t keycode, bool pressed)
{
    const uint16_t now = tick_ms();

    if (keycode >= KC_MS_BTN1 && keycode <= KC_MS_BTN5) {
        const uint8_t bit = (uint8_t)(1u << (keycode - KC_MS_BTN1));
        buttons = pressed ? (uint8_t)(buttons | bit) : (uint8_t)(buttons & ~bit);
        send(0, 0, 0, 0);
        return false;
    }
    if (keycode >= KC_MS_WH_UP && keycode <= KC_MS_WH_RIGHT) {
        const uint8_t bit = (uint8_t)(1u << (keycode - KC_MS_WH_UP));
        if (pressed) {
            wheel_dir  = (uint8_t)(wheel_dir | bit);
            wheel_t0   = now;
            wheel_next = (uint16_t)(now + WHEEL_DELAY_MS);
            wheel_once();
        } else {
            wheel_dir = (uint8_t)(wheel_dir & ~bit);
        }
        return false;
    }
    if (keycode >= KC_MS_UP && keycode <= KC_MS_RIGHT) {
        const uint8_t bit = (uint8_t)(1u << (keycode - KC_MS_UP));
        if (pressed) {
            if (!move_dir) {
                move_t0   = now;
                move_next = now;
            }
            move_dir = (uint8_t)(move_dir | bit);
        } else {
            move_dir = (uint8_t)(move_dir & ~bit);
        }
        return false;
    }
    if (keycode == DBLCLK_L || keycode == DBLCLK_R) {
        if (pressed && !dbl_step) {
            dbl_btn  = (keycode == DBLCLK_L) ? 0x01u : 0x02u;
            dbl_step = 1;
            dbl_next = now;
        }
        return false;
    }
    return true;
}

void aula_mouse_task(void)
{
    const uint16_t now = tick_ms();

    /* スクロール: 押し続けている間、くり返す */
    if (wheel_dir && (int16_t)(now - wheel_next) >= 0) {
        wheel_once();
        wheel_next = (uint16_t)(now + WHEEL_REPEAT_MS);
    }

    /* カーソル移動: 押している間、だんだん速く */
    if (move_dir && (int16_t)(now - move_next) >= 0) {
        uint16_t       held  = (uint16_t)(now - move_t0);
        const uint16_t span  = MOVE_SPEED_MAX - MOVE_SPEED_MIN;
        const int16_t  speed = (int16_t)(MOVE_SPEED_MIN + (held >= MOVE_ACCEL_MS ? span : (uint16_t)((uint32_t)span * held / MOVE_ACCEL_MS)));
        const int16_t  x     = (int16_t)(((move_dir & 8) ? speed : 0) - ((move_dir & 4) ? speed : 0));
        const int16_t  y     = (int16_t)(((move_dir & 2) ? speed : 0) - ((move_dir & 1) ? speed : 0));
        send(x, y, 0, 0);
        move_next = (uint16_t)(now + MOVE_INTERVAL);
    }

    /* ダブルクリック: 押す → 離す → 押す → 離す */
    if (dbl_step && (int16_t)(now - dbl_next) >= 0) {
        if (dbl_step & 1) {
            buttons = (uint8_t)(buttons | dbl_btn);
        } else {
            buttons = (uint8_t)(buttons & ~dbl_btn);
        }
        send(0, 0, 0, 0);
        dbl_next = (uint16_t)(now + DBL_GAP_MS);
        dbl_step = (dbl_step >= 4) ? 0 : (uint8_t)(dbl_step + 1);
    }
}
