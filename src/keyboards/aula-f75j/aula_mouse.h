#pragma once

#include <stdint.h>
#include <stdbool.h>

/* AULA F75J : マウス操作（aula_mouse.c）。 */

/* キーの押す・離すを受け取る。マウスのキーなら false を返して、ほかに回さない。 */
bool aula_mouse_process(uint16_t keycode, bool pressed);

/* ふだんのループから呼ぶ（スクロールとカーソル移動のくり返し、ダブルクリック）。 */
void aula_mouse_task(void);
