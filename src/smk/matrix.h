#pragma once

#include <stdint.h>
#include <stdbool.h>

// Posé à true par l'ISR après chaque scan, reposé à false par matrix_task().
// Exposé pour que la boucle principale puisse prioriser les frappes.
extern volatile bool matrix_updated;

void    matrix_init();
uint8_t matrix_task();

void matrix_scan_full();

// Projet AULA F75J : touche virtuelle (molette), couche active, profils.
void    matrix_virtual_key(uint16_t qcode, bool pressed);
uint8_t matrix_active_layer(void);
uint8_t matrix_base_layer(void);
void    matrix_set_profile(uint8_t profile);
