#include "display.h"

// Buffer para a memória do display (128x64 pixels = 1024 bytes)
uint8_t display_buffer[WIDTH * HEIGHT / 8];

void setup_i2c() {
    i2c_init(I2C_PORT, 400000);

    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C); // Define GPIO14 como SDA
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C); // Define GPIO15 como SCL

    gpio_pull_up(SDA_PIN); // Ativa resistor pull-up interno para SDA
    gpio_pull_up(SCL_PIN); // Ativa resistor pull-up interno para SCL
}

void ssd1306_write_command(uint8_t cmd) {
    uint8_t buffer[2] = {0x00, cmd}; // 0x00 = byte de controle para comandos
    i2c_write_blocking(I2C_PORT, SSD1306_ADDR, buffer, 2, false);
}

void ssd1306_write_data(uint8_t *data, uint16_t len) {
    uint8_t buffer[len + 1];
    buffer[0] = 0x40; // 0x40 = byte de controle para dados
    memcpy(buffer + 1, data, len);
    i2c_write_blocking(I2C_PORT, SSD1306_ADDR, buffer, len + 1, false);
}

void ssd1306_init() {
    // Lista de comandos de inicialização (consultar datasheet do SSD1306)
    // uint8_t init_sequence[] = {
    //     0xAE,       // Desliga o display
    //     0xD5, 0x80, // Configura frequência do clock
    //     0xA8, 0x3F, // Configura multiplexação (64 linhas)
    //     0xD3, 0x00, // Offset de display
    //     0x40,       // Inicia linha 0
    //     0x8D, 0x14, // Habilita carregador de bomba
    //     0x20, 0x00, // Modo de endereçamento horizontal
    //     0xA1,       // Inverte segmentos (horizontal)
    //     0xC8,       // Inverte direção do scan (vertical)
    //     0xDA, 0x12, // Configura pins de hardware
    //     0x81, 0xCF, // Contraste máximo
    //     0xD9, 0xF1, // Pré-carregamento
    //     0xDB, 0x40, // Ajuste VCOMH
    //     0xA4,       // Mostra conteúdo da RAM
    //     0xA6,       // Display normal (não invertido)
    //     0xAF        // Liga o display
    // };

    uint8_t init_sequence[] = {
    // Passo 1: Configuração básica
    0xAE,       // Desliga o display :cite[5]:cite[9]
    0xD5, 0x80, // Define clock (0x80 = ratio padrão) :cite[5]:cite[9]
    0xA8, 0x3F, // Multiplex ratio para 128x64 (0x3F = 63+1) :cite[5]
    0xD3, 0x00, // Offset de display = 0 :cite[9]
    0x40,       // Define linha inicial = 0 :cite[9]
    0x8D, 0x14, // Habilita charge pump (0x14 para VCC interno) :cite[5]:cite[9]
    0x20, 0x00, // Modo de endereçamento horizontal :cite[9]
    0xA1,       // Segment re-map (0xA1 para espelhar horizontalmente) :cite[5]
    0xC8,       // COM scan direction (0xC8 para espelhar verticalmente) :cite[5]
    
    // Passo 2: Configuração de hardware
    0xDA, 0x12, // Configuração de pinos COM (0x12 para 128x64) :cite[5]
    0x81, 0xFF, // Contraste máximo (0xFF) :cite[9]
    0xD9, 0xF1, // Pré-carregamento (0xF1 para fase1=15, fase2=1) :cite[5]
    0xDB, 0x40, // VCOMH deselect level (0x40 = ~0.77V) :cite[5]:cite[9]
    
    // Passo 3: Área de atualização
    0x21, 0x00, 0x7F, // Define colunas (0 a 127) :cite[9]
    0x22, 0x00, 0x07, // Define páginas (0 a 7 para 64 linhas) :cite[9]
    
    // Passo 4: Finalização
    0xA4,       // Exibe conteúdo da RAM (não força todos pixels ligados) :cite[9]
    0xA6,       // Modo de exibição normal (não invertido) :cite[5]
    0xAF        // Liga o display :cite[5]:cite[9]
    };

    printf("rolando init display");

    // Envia todos os comandos
    for (uint16_t i = 0; i < sizeof(init_sequence); i++) {
        ssd1306_write_command(init_sequence[i]);
    }

    printf("terminou init display");
}

void ssd1306_clear() {
    memset(display_buffer, 0, sizeof(display_buffer));
}

void ssd1306_update() {
    // Configura endereçamento para página 0, coluna 0
    ssd1306_write_command(0x21); // Define coluna (0 a 127)
    ssd1306_write_command(0x00);
    ssd1306_write_command(0x7F);
    ssd1306_write_command(0x22); // Define página (0 a 7)
    ssd1306_write_command(0x00);
    ssd1306_write_command(0x07);

    // Envia todo o buffer
    ssd1306_write_data(display_buffer, sizeof(display_buffer));
}

void ssd1306_draw_pixel(uint8_t x, uint8_t y, bool color) {
    if (x >= WIDTH || y >= HEIGHT) return;

    uint16_t page = y / 8;
    uint8_t bit = y % 8;

    if (color) {
        display_buffer[x + (page * WIDTH)] |= (1 << bit);
    } else {
        display_buffer[x + (page * WIDTH)] &= ~(1 << bit);
    }
}

void ssd1306_draw_char(uint8_t x, uint8_t y, char c) {
    const uint8_t *font = get_font(c); // Sua função para obter a matriz de bits
    if (!font) return;

    for (uint8_t linha = 0; linha < 5; linha++) {
        for (uint8_t coluna = 0; coluna < 5; coluna++) {
            if (font[linha] & (1 << (4 - coluna))) {
                ssd1306_draw_pixel(x + coluna, y + linha, 1);
            }
        }
    }
}