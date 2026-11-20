/*
 * AULA F75J（JIS）のライトの表と、その読み出し。
 *
 * 表はすべて tools/gen_light_tables.py（AULA F75J プロジェクト）が、プロジェクトの
 * キー配置と数式から作ったもの（aula_fx_tables.h）。出荷時のファームの表は使っていない。
 * 光り方の動き（どの表をいつ読むか）は layouts/default/indicators.c にある。
 */
#include "aula_fx.h"
#include "aula_fx_tables.h"

void aula_fx_color(uint8_t mode, uint8_t out[3])
{
    const uint8_t i = (mode < (uint8_t)(AULA_FX_COLOR_MODES - 1)) ? mode : 0;

    out[0] = colors[i][0];
    out[1] = colors[i][1];
    out[2] = colors[i][2];
}

void aula_fx_palette(uint8_t index, uint8_t out[3])
{
    const uint8_t i = (uint8_t)(index & (uint8_t)(AULA_FX_PALETTE_SIZE - 1));

    out[0] = palette[i][0];
    out[1] = palette[i][1];
    out[2] = palette[i][2];
}

uint8_t aula_fx_ring(uint8_t ring, uint8_t slot)
{
    if (ring >= AULA_FX_RINGS || slot >= AULA_FX_RING_SLOTS) {
        return 0xff;
    }
    return rings[ring][slot];
}

uint8_t aula_fx_key_id(uint8_t col, uint8_t row)
{
    if (col >= AULA_FX_COLS || row >= AULA_FX_ROWS) {
        return AULA_FX_NO_KEY;
    }
    return key_id[col][row];
}

uint8_t aula_fx_render_col(uint8_t col, uint8_t row)
{
    const uint8_t id = aula_fx_key_id(col, row);

    return (id == AULA_FX_NO_KEY) ? col : (uint8_t)(id >> 3);
}

uint8_t aula_fx_gaming(uint8_t col)
{
    return (col < AULA_FX_COLS) ? gaming[col] : 0;
}

uint8_t aula_fx_shimmer_phase(uint8_t col, uint8_t row, uint8_t rainbow)
{
    if (col >= AULA_FX_COLS || row >= AULA_FX_ROWS) {
        return 0;
    }
    return rainbow ? shimmer_wheel[col][row] : shimmer_fixed[col][row];
}

uint8_t aula_fx_breath(uint8_t phase)
{
    return breath[phase % AULA_FX_BREATH_SIZE];
}

uint8_t aula_fx_keywave(uint8_t col, uint8_t row)
{
    if (col >= AULA_FX_COLS || row >= AULA_FX_ROWS) {
        return 0;
    }
    return keywave[col][row];
}

/* 乱数（0〜255）。8 ビットの xorshift。 */
static uint8_t rand_state = 0x5c;

uint8_t aula_fx_rand(void)
{
    rand_state ^= (uint8_t)(rand_state << 1);
    rand_state ^= (uint8_t)(rand_state >> 1);
    rand_state ^= (uint8_t)(rand_state << 2);
    return rand_state;
}
