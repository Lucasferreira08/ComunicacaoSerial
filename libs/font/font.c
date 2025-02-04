#include "font.h"

// Definições das matrizes de caracteres
// Adicionando caracteres minúsculos à biblioteca font.h

// Caractere 'a'
const uint8_t font_lowercase_a[5] = {
    0b01110,
    0b10001,
    0b10001,
    0b01111,
    0b00001
};

// Caractere 'b'
const uint8_t font_lowercase_b[5] = {
    0b10000,
    0b10000,
    0b11110,
    0b10001,
    0b11110
};

// Caractere 'c'
const uint8_t font_lowercase_c[5] = {
    0b01110,
    0b10001,
    0b10000,
    0b10001,
    0b01110
};

// Caractere 'd'
const uint8_t font_lowercase_d[5] = {
    0b00001,
    0b00001,
    0b01111,
    0b10001,
    0b01111
};

// Caractere 'e'
const uint8_t font_lowercase_e[5] = {
    0b01110,
    0b10001,
    0b11111,
    0b10000,
    0b01110
};

// Caractere 'f'
const uint8_t font_lowercase_f[5] = {
    0b00110,
    0b01001,
    0b01000,
    0b11100,
    0b01000
};

// Caractere 'g'
const uint8_t font_lowercase_g[5] = {
    0b01111,
    0b10001,
    0b01111,
    0b00001,
    0b01110
};

// Caractere 'h'
const uint8_t font_lowercase_h[5] = {
    0b10000,
    0b10000,
    0b11110,
    0b10001,
    0b10001
};

// Caractere 'i'
const uint8_t font_lowercase_i[5] = {
    0b00100,
    0b00000,
    0b00100,
    0b00100,
    0b00100
};
// ... (definições dos outros caracteres)

// Implementação da função
const uint8_t* get_font(char c) {
    switch (c) {
        case 'a': return font_lowercase_a;
        case 'b': return font_lowercase_b;
        case 'c': return font_lowercase_c;
        case 'd': return font_lowercase_d;
        case 'e': return font_lowercase_e;
        case 'f': return font_lowercase_f;
        case 'g': return font_lowercase_g;
        case 'h': return font_lowercase_h;
        case 'i': return font_lowercase_i;
        default: return NULL;
    }
}