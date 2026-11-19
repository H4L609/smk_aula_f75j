#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "macro_store.h" // MACRO_BLOB_SIZE (champ ci-dessous sous MACRO_STORE_ENABLE)
// LIGHT_APPLY_ENABLE (projet AULA F75J). La bibliothèque user.lib est compilée sans le
// dossier du clavier : elle n'a pas kbdef.h, et n'utilise pas les champs de la structure.
#if __has_include("kbdef.h")
#    include "kbdef.h"
#endif

typedef struct {
    uint8_t led_effect;
    uint8_t led_brightness;
    uint8_t led_speed;
    uint8_t ul_effect;
    uint8_t ul_brightness;
    uint8_t ul_speed;
    uint8_t battery_indicator_on;
    uint8_t rf_link;
#if defined(MACRO_STORE_ENABLE)
    /* Blob macro appendu EN FIN de struct : les champs ci-dessus gardent leurs
     * offsets (conn_mode_setting = ul_effect intact). Absent des autres cartes,
     * donc leur budget NVM n'augmente pas. _Static_assert dans settings.c borne
     * sizeof(user_settings_t) à 255 o (longueur du record NVM sur un octet). */
    uint8_t macro_blob[MACRO_BLOB_SIZE];
#endif
#if defined(LIGHT_APPLY_ENABLE)
    /* Projet AULA F75J : numéro du dernier bloc d'éclairage appliqué (0xEBF0),
     * pour ne l'appliquer qu'une fois par écriture de l'éditeur. */
    uint16_t light_applied;
#endif
} user_settings_t;

extern user_settings_t user_settings;

bool settings_load(void);

void settings_save(void);

void settings_mark_dirty(void);

void settings_task(void);

void settings_save_pre(void);
void settings_save_post(void);

#if DEBUG == 1
void settings_dump(void);
#endif
