#pragma once

#include "kbdef.h"
#include <stdbool.h>

#ifdef KEYMAPS_IN_CODE
// La carte place sa table en mémoire CODE à une adresse fixe (projet AULA F75J).
// SDCC refuse un extern à côté d'une définition __at dans la même unité : le
// layout.c qui la définit pose KEYMAPS_DEFINE_HERE avant ses #include.
#    ifndef KEYMAPS_DEFINE_HERE
extern const __code uint16_t keymaps[][MATRIX_ROWS][MATRIX_COLS];
#    endif
#else
extern const uint16_t keymaps[][MATRIX_ROWS][MATRIX_COLS];
#endif

void set_default_layer(uint8_t layer);
