#pragma once

#include <stdint.h>
#include <stdbool.h>

/* AULA F75J : マクロ（MACRO_1〜MACRO_16）。aula_umacro.c を参照。 */
#define UMACRO_COUNT 16

/* キーの押す・離すを受け取る。マクロのキーなら true 以外（false）を返して、ほかに回さない。 */
bool aula_umacro_process(uint16_t keycode, bool pressed);

/* ふだんのループから呼ぶ。 */
void aula_umacro_task(void);
