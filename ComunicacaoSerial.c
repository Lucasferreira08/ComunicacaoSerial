#include <stdio.h>
#include "hardware/uart.h"
#include "display.h"

#define I2C_PORT i2c1       // Controlador I²C0 do RP2040
#define SDA_PIN 14          // GPIO14 para SDA
#define SCL_PIN 15          // GPIO15 para SCL
#define SSD1306_ADDR 0x3C   // Endereço I²C do display (alternativo: 0x3C)

ssd1306_t ssd; // Inicializa a estrutura do display

void setup_i2c() 
{
    i2c_init(I2C_PORT, 400000);

    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C); // Define GPIO14 como SDA
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C); // Define GPIO15 como SCL

    gpio_pull_up(SDA_PIN); // Ativa resistor pull-up interno para SDA
    gpio_pull_up(SCL_PIN); // Ativa resistor pull-up interno para SCL
}

void setup_display()
{
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, SSD1306_ADDR, I2C_PORT); // Inicializa o display
    ssd1306_config(&ssd); // Configura o display
    ssd1306_send_data(&ssd); // Envia os dados para o display

    // Limpa o display. O display inicia com todos os pixels apagados.
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
}

// int main()
// {
//     stdio_init_all(); // Inicializa stdio via USB/UART
//     setup_i2c();
//     setup_display();

//     printf("Display ready! Type characters...\n");
    
//     while(true) {
//         // Verifica entrada sem bloquear
//         int c = getchar_timeout_us(0);
        
//         if(c != PICO_ERROR_TIMEOUT) {
//             // Limpa display e exibe novo caractere
//             ssd1306_fill(&ssd, false);
//             ssd1306_draw_char(&ssd, (char)c, 0, 0);
//             ssd1306_send_data(&ssd);
            
//             // Ecoa o caractere de volta (opcional)
//             printf("Displayed: %c\n", c);
//         }
//         sleep_ms(10);
//     }
// }

int main()
{
    stdio_init_all();
    setup_i2c();
    setup_display();

    uint8_t x = 0;
    uint8_t y = 0;
    
    printf("Display pronto! Digite algo...\n");
    
    while(true) {
        int c = getchar_timeout_us(0);
        
        if(c != PICO_ERROR_TIMEOUT) {
            // Desenha o novo caractere na posição atual
            ssd1306_draw_char(&ssd, (char)c, x, y);
            ssd1306_send_data(&ssd);
            
            // Atualiza a posição para o próximo caractere
            x += 8;
            
            // Quebra de linha
            if(x + 8 >= ssd.width) {
                x = 0;
                y += 8;
                
                // Reset ao chegar no final do display
                if(y + 8 >= ssd.height) {
                    y = 0;
                    ssd1306_fill(&ssd, false); // Limpa ao chegar no final
                }
            }
            
            printf("Displayed: %c em (%d,%d)\n", c, x, y);
        }
        sleep_ms(10);
    }
}