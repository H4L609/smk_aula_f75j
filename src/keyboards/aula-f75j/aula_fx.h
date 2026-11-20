#pragma once

#include <stdint.h>

/*
 * ライトの光り方で使う表の読み出し。
 *
 * 表の中身は aula_fx_tables.h（tools/gen_light_tables.py が、プロジェクトのキー配置と
 * 数式から作る）。光り方の動きは layouts/default/indicators.c にある。
 *
 * キーの番号は「列 * 8 + 行」で表し、0xFF は「キーなし」。
 */

#define AULA_FX_COLS 17
#define AULA_FX_ROWS 6

/* グラデーション: 128 色の (R, G, B)。流れる波で使う。 */
#define AULA_FX_PALETTE_SIZE 128
void aula_fx_palette(uint8_t index, uint8_t out[3]);

/* 色の選び方: 0〜6 は単色（赤・緑・青・黄・紫・水色・白）、7 は虹色（色の輪を使う）。 */
#define AULA_FX_COLOR_MODES  8
#define AULA_FX_COLOR_WHEEL  ((uint8_t)(AULA_FX_COLOR_MODES - 1))
void aula_fx_color(uint8_t mode, uint8_t out[3]);

/* 波紋: 9 つの輪 × 13 か所。中身は「配線の列 * 8 + 行」、0xFF は空き。 */
#define AULA_FX_RINGS      9
#define AULA_FX_RING_SLOTS 13
uint8_t aula_fx_ring(uint8_t ring, uint8_t slot);

/* 配線の (列, 行) → 「見た目の列 * 8 + 行」。キーがなければ AULA_FX_NO_KEY。 */
#define AULA_FX_NO_KEY ((uint8_t)0xFF)
uint8_t aula_fx_key_id(uint8_t col, uint8_t row);

/* 見た目の列（キーの中心が左から何キー目か）。左右に流れる光り方で使う。 */
uint8_t aula_fx_render_col(uint8_t col, uint8_t row);

/* ゲーミング: 光らせるキー（Esc、W A S D、矢印）の行のビット（列ごと）。 */
uint8_t aula_fx_gaming(uint8_t col);

/* きらめき: キーごとの光る時期。rainbow が 0 以外なら 0〜191、0 なら 0〜95。 */
uint8_t aula_fx_shimmer_phase(uint8_t col, uint8_t row, uint8_t rainbow);

/* 呼吸: 96 段の明るさ（20 → 255 → 20）。 */
#define AULA_FX_BREATH_SIZE 96
uint8_t aula_fx_breath(uint8_t phase);

/* 流れる波: キーボードの真ん中のまわりの角度（0〜127）。 */
uint8_t aula_fx_keywave(uint8_t col, uint8_t row);

/* 乱数（0〜255）。 */
uint8_t aula_fx_rand(void);
