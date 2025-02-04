#include <stdio.h>
#include "hardware/uart.h"
#include "display.h"

int main()
{
    char c;
    stdio_init_all(); // Configura UART para stdin/stdout (printf/getchar)

    setup_i2c();      // Configura I²C
    ssd1306_init();   // Inicializa o display
    ssd1306_clear();  // Limpa o display

    printf("Pronto para receber caracteres (a-i)...\n");

    while (1) {
        int received_char = getchar_timeout_us(0); // Lê caractere sem bloquear
        printf("Entrou\n");
        if (received_char != PICO_ERROR_TIMEOUT) { // Se um caractere foi recebido
            if (received_char >= 'a' && received_char <= 'i') {
                ssd1306_clear();
                ssd1306_draw_char(50, 20, (char)received_char); // Exibe no display
                ssd1306_update();
                printf("Caractere exibido: %c\n", received_char); // Feedback via UART
            }
        }
    }

    // setup_i2c(); // Sua função de configuração do I²C

    // scanf("%c", c);

    // printf("Escaneando barramento I2C...\n");
    // for (int addr = 0x08; addr <= 0x77; addr++) {
    //     int result = i2c_write_blocking(I2C_PORT, addr, NULL, 0, false);
    //     if (result >= 0) {
    //         printf("Dispositivo encontrado em 0x%02X\n", addr);
    //     }
    // }
    // while (1);
}
