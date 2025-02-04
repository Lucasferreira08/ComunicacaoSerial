#ifndef FONT_H
#define FONT_H

#include <stdint.h>
#include <stddef.h>

// Declarações das matrizes de caracteres
extern const uint8_t font_lowercase_a[5];
extern const uint8_t font_lowercase_b[5];
extern const uint8_t font_lowercase_c[5];
extern const uint8_t font_lowercase_d[5];
extern const uint8_t font_lowercase_e[5];
extern const uint8_t font_lowercase_f[5];
extern const uint8_t font_lowercase_g[5];
extern const uint8_t font_lowercase_h[5];
extern const uint8_t font_lowercase_i[5];
// ... (declarações dos outros caracteres)

// Declaração da função (sem implementação)
const uint8_t* get_font(char c);

#endif