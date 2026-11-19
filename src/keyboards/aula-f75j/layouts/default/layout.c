#define KEYMAPS_DEFINE_HERE 1 /* layout.h : pas d'extern pour keymaps ici */
#include "kbdef.h"
#include "layout.h"
#include "aula_umacro.h"
#include "user_layout.h"
#include "report.h"
#include <stdint.h>

/*
 * AULA F75J (JIS) -- matrice 6 x 17.
 *
 * Disposition relue dans la couche de base d'usine du F75J (CODE 0xCC00,
 * 4 octets par touche, index = colonne * 6 + ligne). Voir le projet AULA F75J :
 * docs/f75j-matrix.md et docs/firmware-analysis.md.
 *
 * Différences avec le F75 ANSI :
 *   r1 c16 = ¥ (colonne 16 = P4.6, absente sur l'ANSI)
 *   r2 c13 = vide (Entrée ISO)       r3 c12 = ]
 *   r4 c11 = ろ   r4 c12 = Maj droite
 *   r5 c3  = 無変換  r5 c7 = 変換  r5 c8 = かな  r5 c9 = Fn (pas de Ctrl droit)
 *   c15 = colonne fantôme (P4.5 = sélecteur de connexion), jamais pilotée.
 *
 * Codes HID côté JIS (l'hôte Windows réglé en clavier japonais les interprète) :
 *   半角/全角 = KC_GRV (0x35)   ^ = KC_EQL (0x2E)   @ = KC_LBRC (0x2F)
 *   [ = KC_RBRC (0x30)         : = KC_QUOT (0x34)  ] = KC_BSLS (0x31)
 *   ろ = KC_INT1  かな = KC_INT2  ¥ = KC_INT3  変換 = KC_INT4  無変換 = KC_INT5
 */

// clang-format off

#define LAYOUT_75_jis( \
    K00_0,        K02_0, K03_0, K04_0, K05_0, K06_0, K07_0, K08_0, K09_0, K10_0, K11_0, K12_0, K13_0, K14_0, \
    K00_1, K01_1, K02_1, K03_1, K04_1, K05_1, K06_1, K07_1, K08_1, K09_1, K10_1, K11_1, K12_1, K16_1, K13_1, K14_1, \
    K00_2, K01_2, K02_2, K03_2, K04_2, K05_2, K06_2, K07_2, K08_2, K09_2, K10_2, K11_2, K12_2,        K14_2, \
    K00_3, K01_3, K02_3, K03_3, K04_3, K05_3, K06_3, K07_3, K08_3, K09_3, K10_3, K11_3, K12_3, K13_3, K14_3, \
    K00_4, K01_4, K02_4, K03_4, K04_4, K05_4, K06_4, K07_4, K08_4, K09_4, K10_4, K11_4, K12_4, K13_4, K14_4, \
    K00_5, K01_5, K02_5, K03_5,        K05_5,        K07_5, K08_5, K09_5,               K12_5, K13_5, K14_5  \
) { \
    { K00_0, KC_NO, K02_0, K03_0, K04_0, K05_0, K06_0, K07_0, K08_0, K09_0, K10_0, K11_0, K12_0, K13_0, K14_0, KC_NO, KC_NO }, \
    { K00_1, K01_1, K02_1, K03_1, K04_1, K05_1, K06_1, K07_1, K08_1, K09_1, K10_1, K11_1, K12_1, K13_1, K14_1, KC_NO, K16_1 }, \
    { K00_2, K01_2, K02_2, K03_2, K04_2, K05_2, K06_2, K07_2, K08_2, K09_2, K10_2, K11_2, K12_2, KC_NO, K14_2, KC_NO, KC_NO }, \
    { K00_3, K01_3, K02_3, K03_3, K04_3, K05_3, K06_3, K07_3, K08_3, K09_3, K10_3, K11_3, K12_3, K13_3, K14_3, KC_NO, KC_NO }, \
    { K00_4, K01_4, K02_4, K03_4, K04_4, K05_4, K06_4, K07_4, K08_4, K09_4, K10_4, K11_4, K12_4, K13_4, K14_4, KC_NO, KC_NO }, \
    { K00_5, K01_5, K02_5, K03_5, KC_NO, K05_5, KC_NO, K07_5, K08_5, K09_5, KC_NO, KC_NO, K12_5, K13_5, K14_5, KC_NO, KC_NO }  \
}

/*
 * キー表の置き場所を固定する（AULA F75J キー配置エディタ用）。
 *
 * エディタは、ビルド済みのファームの中のこの場所にキー表を直接書き込む。
 * そのため、使う人の PC には組み立て用の道具が要らない。
 *
 *   0xE000  見出し 16 バイト: "F75JKMAP", 版 4, 行数, 列数, 層の数 8, プロファイル数 4, マクロ数 16,
 *           タップと長押しを見分ける時間 ms（2 バイト、リトルエンディアン。0 なら 200）
 *   0xE010  keymaps[8][6][17]（uint16_t、リトルエンディアン）= 1632 バイト
 *           プロファイル p の層 = 2p（ベース）と 2p+1（Fn）
 *   0xE670  encoder_map[8][2]（層ごとに [左回し, 右回し]）= 32 バイト
 *   0xE690  umacro_dir[16][2]（マクロごとに [手順の開始位置, 繰り返し方]）= 64 バイト
 *   0xE6D0  umacro_steps[328][2]（[種類, 値]）= 1312 バイト（0xEBEF まで）
 *   0xEBF0  light_block[16]: 'L', 版 1, 反映する(0/1), 光り方, 明るさ, 速さ, 色, 向き, 書き込みの番号(2), 予備 6
 *
 * 0xEC00 からは設定の保存場所（--code-size で code はその手前まで）。
 * 通常のコードが 0xE000 に届かないことは、tools/check_layout.py で確かめる。
 * 中身の初期値は keymap/keymap.json から tools/keymap_gen.py が作る（keymap_user.h）。
 */
#define KEYMAP_LAYERS_MAX 8

#define UMACRO_STEPS_MAX 328
#include "keymap_user.h"

const __code __at(0xE000) uint8_t keymap_header[16] = {
    'F', '7', '5', 'J', 'K', 'M', 'A', 'P', 4, MATRIX_ROWS, MATRIX_COLS, KEYMAP_LAYERS_MAX, KEYMAP_PROFILES, UMACRO_COUNT,
    (uint8_t)(TAPPING_TERM_MS & 0xFF), (uint8_t)(TAPPING_TERM_MS >> 8),
};


const __code __at(0xE010) uint16_t keymaps[KEYMAP_LAYERS_MAX][MATRIX_ROWS][MATRIX_COLS] = KEYMAP_INIT;

const __code __at(0xE670) uint16_t encoder_map[KEYMAP_LAYERS_MAX][2] = ENCODER_INIT;

const __code __at(0xE690) uint16_t umacro_dir[UMACRO_COUNT][2] = UMACRO_DIR_INIT;

const __code __at(0xE6D0) uint16_t umacro_steps[UMACRO_STEPS_MAX][2] = UMACRO_STEPS_INIT;

const __code __at(0xEBF0) uint8_t light_block[16] = LIGHT_INIT;

/*
 * 設定ファイルの置き場所（AULA F75J キー配置エディタ用）。0xD000〜0xDFFF の 4KB。
 *
 * エディタは書き込むときに、ここへ設定ファイル（keymap.json を圧縮したもの、名前なども全部）を
 * 入れる。別の PC のエディタで「キーボードから読み込む」と、ここを読み出して元の設定に戻す。
 * ファーム自身は読まない（エディタのための保管場所）。
 *
 *   0xD000  "F75JCONF"（8）, 版 1, 予備 1, 長さ（2、LE）, CRC32（4、LE）, 圧縮した JSON（最大 4080）
 *
 * 通常のコードが 0xD000 に届かないことは、tools/check_layout.py で確かめる。
 */
const __code __at(0xD000) uint8_t config_store[4096] = {
    'F', '7', '5', 'J', 'C', 'O', 'N', 'F', 1, 0, 0, 0, 0, 0, 0, 0,
};
