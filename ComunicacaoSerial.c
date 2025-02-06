#include <stdio.h>
#include "hardware/uart.h"
#include "display.h"
#include "matriz_led.h"

#define I2C_PORT i2c1       // Controlador I²C0 do RP2040
#define SDA_PIN 14          // GPIO14 para SDA
#define SCL_PIN 15          // GPIO15 para SCL
#define SSD1306_ADDR 0x3C   // Endereço I²C do display (alternativo: 0x3C)

#define OUT_PIN 7

// Variáveis para o PIO (Programmable I/O) e state machine (máquina de estados)
PIO pio;
uint sm;

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

// Função para configurar o PIO (Programmable I/O)
uint pio_config(PIO pio) 
{
    set_sys_clock_khz(128000, false); // Configura o clock do sistema para 128 MHz

    // Adiciona o programa PIO para controle da matriz de LEDs
    uint offset = pio_add_program(pio, &pio_matrix_program);

    // Obtém uma state machine (máquina de estados) não utilizada
    uint sm = pio_claim_unused_sm(pio, true);

    // Inicializa o programa PIO na state machine com o pino de saída definido
    pio_matrix_program_init(pio, sm, offset, OUT_PIN);

    return sm; // Retorna a state machine configurada
}


int main()
{
    stdio_init_all();
    setup_i2c();
    setup_display();

    pio = pio0;
    sm = pio_config(pio);

    uint8_t x = 0;
    uint8_t y = 0;
    
    printf("Display pronto! Digite algo...\n");

    absolute_time_t last_char_time = get_absolute_time();  // AQUI
    
    while(true) {
        int c = getchar_timeout_us(0);
        
        if(c != PICO_ERROR_TIMEOUT) {

            absolute_time_t now = get_absolute_time();  // AQUI

            // Aplica debounce temporal para evitar leituras muito próximas
            if(absolute_time_diff_us(last_char_time, now) > 50*1000) 
            {
                // Desenha o caractere no display
                ssd1306_draw_char(&ssd, (char)c, x, y);
                ssd1306_send_data(&ssd);

                // Atualiza a posição para o próximo caractere
                x += 8;
                if(x + 8 >= ssd.width) {
                    x = 0;
                    y += 8;
                    if(y + 8 >= ssd.height) {
                        y = 0;
                        ssd1306_fill(&ssd, false);
                    }
                }

                last_char_time = now;
                printf("Displayed: %c em (%d,%d)\n", c, x, y);

                main_animacao((c-'0'), pio, sm);
            }
        }

        sleep_ms(10);
    }
}