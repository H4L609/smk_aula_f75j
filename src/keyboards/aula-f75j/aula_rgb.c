#include "aula_rgb.h"
#include "sh68f90.h"
#include "pwm.h"

/*
 * Ordre de chargement des canaux -- relevé dans fcn @ 0x6E61 du firmware
 * d'usine et recoupé avec la carte des registres du datasheet SH68F90 CV2.0
 * (tableau XDATA 0xFF80-0xFFFF).
 *
 * Le firmware d'usine lit 36 octets consécutifs du framebuffer (base XRAM
 * 0x05A2 + colonne * 36) et les écrit dans cet ordre exact, en alternant les
 * deux DPTR via INSCON. Chaque canal reçoit DUTY2 en GROS-BOUTISTE : l'octet
 * de poids fort d'abord.
 *
 *   octets  0-11  ->  PWM20..PWM25   (P1.0..P1.5)
 *   octets 12-23  ->  PWM10..PWM15   (P2.0..P2.5)
 *   octets 24-29  ->  PWM03,04,05    (P3.3..P3.5)
 *   octets 30-35  ->  PWM00,01,02    (P3.0..P3.2)
 *
 * Noter la ROTATION dans le groupe P3 : PWM03-05 sont chargés AVANT PWM00-02.
 * C'est la seule irrégularité de la table, et elle est délibérée côté usine.
 *
 * CORRESPONDANCE CANAL <-> (LIGNE, COULEUR) -- établie.
 *
 * Le driver OpenRGB de ce clavier (SinowealthKeyboard10cController::SetLEDsDirect)
 * envoie les couleurs linéairement, 3 octets par LED, à l'offset 0x08 + i*3, où
 * i est l'indice LED de la table de disposition, soit colonne * 6 + ligne.
 * Par colonne, l'hôte émet donc 18 octets dans l'ordre
 * ligne0 R,G,B puis ligne1 R,G,B, etc. -- d'où :
 *
 *     canal = ligne * 3 + couleur
 *
 * Combiné à l'ordre de chargement relevé ci-dessus :
 *
 *     lignes 0-1  ->  P1.0 .. P1.5   (PWM20..PWM25)
 *     lignes 2-3  ->  P2.0 .. P2.5   (PWM10..PWM15)
 *     ligne  4    ->  P3.3 .. P3.5   (PWM03..PWM05)
 *     ligne  5    ->  P3.0 .. P3.2   (PWM00..PWM02)
 *
 * Deux lignes par port, six broches chacun : la régularité du résultat est en
 * elle-même un argument, et elle explique la rotation apparente du groupe P3.
 *
 * CE N'EST PLUS UNE INFÉRENCE. Le convertisseur d'usine 0x765B lit le
 * framebuffer 8 bits en `0x0152 + colonne * 18 + ligne * 3 + couleur` et écrit
 * le tampon 16 bits en `0x05A2 + colonne * 36 + ligne * 6 + couleur * 2` : le
 * MÊME indexage `(ligne, couleur)` des deux côtés, sans permutation. Et la
 * table de phases `CODE 0x2922`, indexée elle aussi `ligne * 3 + couleur`, se
 * retrouve octet pour octet dans les DUTY1 posés à l'init -- permutés par
 * exactement l'ordre de chargement ci-dessus. La correspondance est lue, pas
 * supposée ; la rédaction précédente renvoyait à un essai sur matériel qui
 * n'est plus nécessaire.
 */

/*
 * Le cache porte des rapports cycliques DÉJÀ convertis, pas des intensités.
 *
 * La conversion coûte une multiplication ; la faire ici, à l'écriture d'un
 * pixel, c'est six conversions par sous-trame (une cellule d'effet), contre
 * dix-huit si on la faisait au chargement de colonne — et le chargement, lui,
 * tourne dans l'ISR.
 */
static AULA_RGB_XDATA uint16_t duty_cache[AULA_RGB_COLS][AULA_RGB_CHANNELS];

/*
 * Conversion 8 bits -> 16 bits, transcrite du convertisseur d'usine 0x76C3 :
 *
 *     DUTY2 = (valeur << 2) + phase du canal
 *
 * L'impulsion PWM s'étend de DUTY1 à DUTY2 et DUTY1 reste figé sur la phase du
 * canal (voir `aula_rgb.h`), donc la largeur utile vaut exactement `valeur << 2`
 * et la phase ne fait que décaler le front dans la période.
 *
 * ÉCART ASSUMÉ AVEC L'USINE -- l'écrêtage. À valeur 255, la somme vaut
 * 1020 + phase, soit jusqu'à 1217 pour la phase la plus haute (0xC5), donc
 * 17 crans AU-DELÀ de la période de 1200. L'usine ne l'écrête pas ; nous si,
 * parce que `led_effect_rgb()` produit bel et bien 255 et qu'un DUTY2 supérieur
 * à la période n'a pas de comportement défini par le datasheet. L'écrêtage ne
 * mord qu'à partir de la valeur 251 et sur les trois canaux de la ligne 5.
 */
uint16_t aula_rgb_duty(uint8_t channel, uint8_t value)
{
    const uint16_t duty = (uint16_t)(((uint16_t)value << 2) + AULA_RGB_PHASE(channel));

    return (duty > AULA_RGB_PERIOD) ? AULA_RGB_PERIOD : duty;
}


/* ------------------------------------------------------------ 色の輪 */

/*
 * 色の輪（192 色、赤 → 黄 → 緑 → 水色 → 青 → 紫）。tools/gen_light_tables.py（AULA F75J
 * プロジェクト）が数式で作ったもの（aula_rgb_wheel.h）。明るさの立ち上がりは 2 乗で
 * ゆるやかにして、色の変わり目が目立たないようにしている。
 */
#include "aula_rgb_wheel.h"

void aula_rgb_wheel(uint8_t index, uint8_t out[3])
{
    if (index >= AULA_RGB_WHEEL_SIZE) {
        index = (uint8_t)(index % AULA_RGB_WHEEL_SIZE);
    }
    out[0] = wheel[index][0];
    out[1] = wheel[index][1];
    out[2] = wheel[index][2];
}

void aula_rgb_clear(void)
{
    uint8_t col;
    uint8_t ch;
    for (col = 0; col < AULA_RGB_COLS; col++) {
        for (ch = 0; ch < AULA_RGB_CHANNELS; ch++) {
            /* éteint = impulsion nulle = DUTY2 ramené sur la phase du canal */
            duty_cache[col][ch] = AULA_RGB_PHASE(ch);
        }
    }
}

void aula_rgb_set(uint8_t row, uint8_t col, uint8_t red, uint8_t green, uint8_t blue)
{
    if (row >= AULA_RGB_ROWS || col >= AULA_RGB_COLS) {
        return;
    }
    const uint8_t base = (uint8_t)(row * AULA_RGB_COLORS);

    duty_cache[col][base + 0] = aula_rgb_duty((uint8_t)(base + 0), red);
    duty_cache[col][base + 1] = aula_rgb_duty((uint8_t)(base + 1), green);
    duty_cache[col][base + 2] = aula_rgb_duty((uint8_t)(base + 2), blue);
}

/*
 * Les dix-huit écritures sont déroulées avec `SET_PWM_DUTY_2` plutôt que
 * parcourues par une table d'adresses. La rédaction précédente gardait deux
 * tables `static const __xdata uint16_t[18]`, soit 72 octets de XRAM plus leur
 * initialisation en flash, pour finir par des écritures via pointeur générique.
 * Les jetons de `pwm.h` donnent des accès directs, et l'ordre reste celui du
 * firmware d'usine -- rotation du groupe P3 comprise.
 */
void aula_rgb_load_column(uint8_t col)
{
    if (col >= AULA_RGB_COLS) {
        return;
    }

    const AULA_RGB_XDATA uint16_t *const d = duty_cache[col];

    SET_PWM_DUTY_2(PWM20, d[0]);  /* ligne 0 R */
    SET_PWM_DUTY_2(PWM21, d[1]);  /* ligne 0 V */
    SET_PWM_DUTY_2(PWM22, d[2]);  /* ligne 0 B */
    SET_PWM_DUTY_2(PWM23, d[3]);  /* ligne 1 R */
    SET_PWM_DUTY_2(PWM24, d[4]);
    SET_PWM_DUTY_2(PWM25, d[5]);
    SET_PWM_DUTY_2(PWM10, d[6]);  /* ligne 2 R */
    SET_PWM_DUTY_2(PWM11, d[7]);
    SET_PWM_DUTY_2(PWM12, d[8]);
    SET_PWM_DUTY_2(PWM13, d[9]);  /* ligne 3 R */
    SET_PWM_DUTY_2(PWM14, d[10]);
    SET_PWM_DUTY_2(PWM15, d[11]);
    SET_PWM_DUTY_2(PWM03, d[12]); /* ligne 4 R -- le groupe P3 est chargé */
    SET_PWM_DUTY_2(PWM04, d[13]); /* dans l'ordre 03,04,05 PUIS 00,01,02, */
    SET_PWM_DUTY_2(PWM05, d[14]); /* seule irrégularité de la table d'usine */
    SET_PWM_DUTY_2(PWM00, d[15]); /* ligne 5 R */
    SET_PWM_DUTY_2(PWM01, d[16]);
    SET_PWM_DUTY_2(PWM02, d[17]);
}
