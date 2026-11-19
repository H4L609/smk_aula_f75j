// このファイルは tools/keymap_gen.py が keymap/keymap.json から作ったもの。
// 直接書き換えないこと（次のビルドで上書きされる）。
#define KEYMAP_INIT { \
    /* 0: プロファイル 1 base */ { \
        { KC_ESC, KC_NO, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12, KC_MUTE, KC_NO, KC_NO }, \
        { KC_GRV, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_MINS, KC_EQL, KC_BSPC, KC_DEL, KC_NO, KC_INT3 }, \
        { KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_LBRC, KC_RBRC, KC_NO, KC_PGUP, KC_NO, KC_NO }, \
        { (QK_MOD_TAP | (0x01 << 8) | KC_ESC), KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_QUOT, KC_BSLS, KC_ENT, KC_PGDN, KC_NO, KC_NO }, \
        { KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_INT1, KC_RSFT, KC_UP, KC_END, KC_NO, KC_NO }, \
        { KC_LCTL, KC_LGUI, KC_LALT, KC_INT5, KC_NO, KC_SPC, KC_NO, KC_INT4, KC_INT2, MO(1), KC_NO, KC_NO, KC_LEFT, KC_DOWN, KC_RGHT, KC_NO, KC_NO }, \
    }, \
    /* 1: プロファイル 1 fn */ { \
        { BOOT_HOLD, KC_NO, KC_WHOM, KC_MAIL, LALT(KC_TAB), LGUI(KC_E), BRI_DN, BRI_UP, KC_MPRV, KC_MPLY, KC_MNXT, KC_MUTE, KC_VOLD, KC_VOLU, MACRO_TG, KC_NO, KC_NO }, \
        { RF_PAIR, LNK_BT1, LNK_BT2, LNK_BT3, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_INS, KC_NO, _______ }, \
        { FX_NEXT, _______, _______, _______, RF_DIAG, _______, _______, KC_PSCR, KC_SCRL, KC_PAUS, _______, _______, _______, KC_NO, _______, KC_NO, KC_NO }, \
        { _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, CLR_NEXT, _______, _______, KC_NO, KC_NO }, \
        { DIR_TOG, _______, _______, _______, _______, _______, _______, _______, FX_PREV, FX_NEXT, _______, _______, _______, BRI_UP, KC_HOME, KC_NO, KC_NO }, \
        { _______, _______, _______, _______, KC_NO, MACRO_TG, KC_NO, _______, _______, _______, KC_NO, KC_NO, KC_HOME, BRI_DN, KC_END, KC_NO, KC_NO }, \
    }, \
    /* 2: プロファイル 2 base */ { \
        { KC_ESC, KC_NO, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12, KC_MUTE, KC_NO, KC_NO }, \
        { KC_GRV, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_MINS, KC_EQL, KC_BSPC, KC_DEL, KC_NO, KC_INT3 }, \
        { KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_LBRC, KC_RBRC, KC_NO, KC_PGUP, KC_NO, KC_NO }, \
        { (QK_MOD_TAP | (0x01 << 8) | KC_ESC), KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_QUOT, KC_BSLS, KC_ENT, KC_PGDN, KC_NO, KC_NO }, \
        { KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_INT1, KC_RSFT, KC_UP, KC_END, KC_NO, KC_NO }, \
        { KC_LCTL, KC_LGUI, KC_LALT, KC_INT5, KC_NO, KC_SPC, KC_NO, KC_INT4, KC_INT2, MO(1), KC_NO, KC_NO, KC_LEFT, KC_DOWN, KC_RGHT, KC_NO, KC_NO }, \
    }, \
    /* 3: プロファイル 2 fn */ { \
        { BOOT_HOLD, KC_NO, KC_WHOM, KC_MAIL, LALT(KC_TAB), LGUI(KC_E), BRI_DN, BRI_UP, KC_MPRV, KC_MPLY, KC_MNXT, KC_MUTE, KC_VOLD, KC_VOLU, MACRO_TG, KC_NO, KC_NO }, \
        { RF_PAIR, LNK_BT1, LNK_BT2, LNK_BT3, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_INS, KC_NO, _______ }, \
        { FX_NEXT, _______, _______, _______, RF_DIAG, _______, _______, KC_PSCR, KC_SCRL, KC_PAUS, _______, _______, _______, KC_NO, _______, KC_NO, KC_NO }, \
        { _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, CLR_NEXT, _______, _______, KC_NO, KC_NO }, \
        { DIR_TOG, _______, _______, _______, _______, _______, _______, _______, FX_PREV, FX_NEXT, _______, _______, _______, BRI_UP, KC_HOME, KC_NO, KC_NO }, \
        { _______, _______, _______, _______, KC_NO, MACRO_TG, KC_NO, _______, _______, _______, KC_NO, KC_NO, KC_HOME, BRI_DN, KC_END, KC_NO, KC_NO }, \
    }, \
    /* 4: プロファイル 3 base */ { \
        { KC_ESC, KC_NO, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12, KC_MUTE, KC_NO, KC_NO }, \
        { KC_GRV, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_MINS, KC_EQL, KC_BSPC, KC_DEL, KC_NO, KC_INT3 }, \
        { KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_LBRC, KC_RBRC, KC_NO, KC_PGUP, KC_NO, KC_NO }, \
        { (QK_MOD_TAP | (0x01 << 8) | KC_ESC), KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_QUOT, KC_BSLS, KC_ENT, KC_PGDN, KC_NO, KC_NO }, \
        { KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_INT1, KC_RSFT, KC_UP, KC_END, KC_NO, KC_NO }, \
        { KC_LCTL, KC_LGUI, KC_LALT, KC_INT5, KC_NO, KC_SPC, KC_NO, KC_INT4, KC_INT2, MO(1), KC_NO, KC_NO, KC_LEFT, KC_DOWN, KC_RGHT, KC_NO, KC_NO }, \
    }, \
    /* 5: プロファイル 3 fn */ { \
        { BOOT_HOLD, KC_NO, KC_WHOM, KC_MAIL, LALT(KC_TAB), LGUI(KC_E), BRI_DN, BRI_UP, KC_MPRV, KC_MPLY, KC_MNXT, KC_MUTE, KC_VOLD, KC_VOLU, MACRO_TG, KC_NO, KC_NO }, \
        { RF_PAIR, LNK_BT1, LNK_BT2, LNK_BT3, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_INS, KC_NO, _______ }, \
        { FX_NEXT, _______, _______, _______, RF_DIAG, _______, _______, KC_PSCR, KC_SCRL, KC_PAUS, _______, _______, _______, KC_NO, _______, KC_NO, KC_NO }, \
        { _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, CLR_NEXT, _______, _______, KC_NO, KC_NO }, \
        { DIR_TOG, _______, _______, _______, _______, _______, _______, _______, FX_PREV, FX_NEXT, _______, _______, _______, BRI_UP, KC_HOME, KC_NO, KC_NO }, \
        { _______, _______, _______, _______, KC_NO, MACRO_TG, KC_NO, _______, _______, _______, KC_NO, KC_NO, KC_HOME, BRI_DN, KC_END, KC_NO, KC_NO }, \
    }, \
    /* 6: プロファイル 4 base */ { \
        { KC_ESC, KC_NO, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12, KC_MUTE, KC_NO, KC_NO }, \
        { KC_GRV, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_MINS, KC_EQL, KC_BSPC, KC_DEL, KC_NO, KC_INT3 }, \
        { KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_LBRC, KC_RBRC, KC_NO, KC_PGUP, KC_NO, KC_NO }, \
        { (QK_MOD_TAP | (0x01 << 8) | KC_ESC), KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_QUOT, KC_BSLS, KC_ENT, KC_PGDN, KC_NO, KC_NO }, \
        { KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_INT1, KC_RSFT, KC_UP, KC_END, KC_NO, KC_NO }, \
        { KC_LCTL, KC_LGUI, KC_LALT, KC_INT5, KC_NO, KC_SPC, KC_NO, KC_INT4, KC_INT2, MO(1), KC_NO, KC_NO, KC_LEFT, KC_DOWN, KC_RGHT, KC_NO, KC_NO }, \
    }, \
    /* 7: プロファイル 4 fn */ { \
        { BOOT_HOLD, KC_NO, KC_WHOM, KC_MAIL, LALT(KC_TAB), LGUI(KC_E), BRI_DN, BRI_UP, KC_MPRV, KC_MPLY, KC_MNXT, KC_MUTE, KC_VOLD, KC_VOLU, MACRO_TG, KC_NO, KC_NO }, \
        { RF_PAIR, LNK_BT1, LNK_BT2, LNK_BT3, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_INS, KC_NO, _______ }, \
        { FX_NEXT, _______, _______, _______, RF_DIAG, _______, _______, KC_PSCR, KC_SCRL, KC_PAUS, _______, _______, _______, KC_NO, _______, KC_NO, KC_NO }, \
        { _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, CLR_NEXT, _______, _______, KC_NO, KC_NO }, \
        { DIR_TOG, _______, _______, _______, _______, _______, _______, _______, FX_PREV, FX_NEXT, _______, _______, _______, BRI_UP, KC_HOME, KC_NO, KC_NO }, \
        { _______, _______, _______, _______, KC_NO, MACRO_TG, KC_NO, _______, _______, _______, KC_NO, KC_NO, KC_HOME, BRI_DN, KC_END, KC_NO, KC_NO }, \
    }, \
}
#define ENCODER_INIT { \
    { KC_VOLD, KC_VOLU }, \
    { _______, _______ }, \
    { KC_VOLD, KC_VOLU }, \
    { _______, _______ }, \
    { KC_VOLD, KC_VOLU }, \
    { _______, _______ }, \
    { KC_VOLD, KC_VOLU }, \
    { _______, _______ }, \
}
#define UMACRO_DIR_INIT { { 0, 0 }, { 15, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } }
#define UMACRO_STEPS_INIT { { 1, 0x020B }, { 1, 0x0008 }, { 1, 0x000F }, { 1, 0x000F }, { 1, 0x0012 }, { 1, 0x0036 }, { 1, 0x002C }, { 1, 0x021A }, { 1, 0x0012 }, { 1, 0x0015 }, { 1, 0x000F }, { 1, 0x0007 }, { 1, 0x021E }, { 1, 0x0028 }, { 0, 0x0000 }, { 1, 0x0106 }, { 4, 0x0032 }, { 1, 0x0119 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 }, { 0, 0x0000 } }
#define LIGHT_INIT { 0x4C, 0x01, 0x00, 0x0A, 0x09, 0x02, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define TAPPING_TERM_MS 200
