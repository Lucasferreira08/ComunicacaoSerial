#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "string.h"
#include "font.h"

// Definições de pinos e endereçamento
#define I2C_PORT i2c0       // Controlador I²C0 do RP2040
#define SDA_PIN 14          // GPIO14 para SDA
#define SCL_PIN 15          // GPIO15 para SCL
#define SSD1306_ADDR 0x3C   // Endereço I²C do display (alternativo: 0x3C)
#define WIDTH 128           // Largura do display em pixels
#define HEIGHT 64           // Altura do display em pixels

// Buffer de memória para os pixels (extern para acesso global)
extern uint8_t display_buffer[WIDTH * HEIGHT / 8];

// Protótipos das funções
void setup_i2c(void);                       // Configura o hardware I²C
void ssd1306_write_command(uint8_t cmd);    // Envia um comando ao display
void ssd1306_write_data(uint8_t *data, uint16_t len); // Envia dados ao display
void ssd1306_init(void);                    // Inicializa o display
void ssd1306_clear(void);                   // Limpa o buffer de pixels
void ssd1306_update(void);                  // Atualiza o display com o buffer
void ssd1306_draw_pixel(uint8_t x, uint8_t y, bool color); // Desenha um pixel
void ssd1306_draw_char(uint8_t x, uint8_t y, char c);      // Desenha um caractere

#endif // DISPLAY_H