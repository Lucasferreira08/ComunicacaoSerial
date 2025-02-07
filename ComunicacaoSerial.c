#include <stdio.h>
#include "hardware/uart.h"
#include "display.h"
#include "matriz_led.h"

#define I2C_PORT i2c1       // Controlador I²C0 do RP2040
#define SDA_PIN 14          // GPIO14 para SDA
#define SCL_PIN 15          // GPIO15 para SCL
#define SSD1306_ADDR 0x3C   // Endereço I²C do display (alternativo: 0x3C)

#define OUT_PIN 7 // Pino PIO

#define BUTTON_A 5 // Define os pinos dos botões A e B
#define BUTTON_B 6 // Define os pinos dos botões A e B
#define OUT_PIN_GREEN 13
#define OUT_PIN_BLUE 12 

static volatile uint32_t last_time = 0; // Armazena o último tempo de interrupção
static volatile uint8_t x = 0;
static volatile uint8_t y = 0;

// Variáveis para o PIO (Programmable I/O) e state machine (máquina de estados)
PIO pio;
uint sm;

ssd1306_t ssd; // Inicializa a estrutura do display

void gpio_irq_handler(uint gpio, uint32_t events)
{
    uint32_t current_time = to_us_since_boot(get_absolute_time()); // Obtém o tempo atual em microssegundos
    ssd1306_fill(&ssd, false); // Limpa o display

    // Verifica se passaram pelo menos 250ms desde a última interrupção (debouncing)
    if (current_time - last_time > 250000) // 250 ms para evitar pulos
    {
        // Se o botão A foi pressionado e 'a' é menor que 9, incrementa 'a'
        if (gpio == BUTTON_A) {
            if (!gpio_get(OUT_PIN_GREEN)) 
            {
                printf("Led verde aceso.\n");
                ssd1306_draw_string(&ssd, "Led verde aceso\n", 0, 0); // Desenha uma string
            }
            else 
            {
                printf("Led verde desligado.\n");
                ssd1306_draw_string(&ssd, "Led verde desligado\n", 0, 0); // Desenha uma string
            }
            gpio_put(OUT_PIN_GREEN, !gpio_get(OUT_PIN_GREEN));
        }
        else if (gpio == BUTTON_B) {
            if (!gpio_get(OUT_PIN_BLUE)) 
            {
                printf("Led azul aceso.\n");
                ssd1306_draw_string(&ssd, "Led azul aceso\n", 0, 0); // Desenha uma string
            }
            else 
            {
                printf("Led azul desligado.\n");
                ssd1306_draw_string(&ssd, "Led azul desligado\n", 0, 0); // Desenha uma string
            }
            gpio_put(OUT_PIN_BLUE, !gpio_get(OUT_PIN_BLUE));
        }
        ssd1306_send_data(&ssd);
        x=0;
        y=0; 
        // Atualiza o último tempo de interrupção
        last_time = current_time;
    }
}

// Função para configurar os pinos
void pinos_config() 
{
    // Configura o pino do botão A como entrada com pull-up
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);

    // Configura o pino do botão B como entrada com pull-up
    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);

    // Configura o pino do LED vermelho como saída
    gpio_init(OUT_PIN_GREEN);
    gpio_set_dir(OUT_PIN_GREEN, GPIO_OUT);

    gpio_init(OUT_PIN_BLUE);
    gpio_set_dir(OUT_PIN_BLUE, GPIO_OUT);

    // Habilita a interrupção para os botões A e B na borda de descida (quando o botão é pressionado)
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
}

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
    pinos_config();

    pio = pio0;
    sm = pio_config(pio);
    
    printf("Display pronto! Digite algo...\n");

    absolute_time_t last_char_time = get_absolute_time();  // AQUI
    
    while(true) {
        int c = getchar_timeout_us(0);
        
        if(c != PICO_ERROR_TIMEOUT) {

            absolute_time_t now = get_absolute_time();  // AQUI

            if (!x && !y)
            {
                ssd1306_fill(&ssd, false);
            }
            

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