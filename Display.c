#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>
#include "hardware/pio.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "ws2812.pio.h"  // Biblioteca para controlar LEDs WS2812
#include "inc/font.h"    // Biblioteca de fontes personalizada
#include "inc/ssd1306.h" // Biblioteca para controlar o display ssd1306

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define ENDERECO 0x3C
#define BUTTON_A_PIN 5
#define BUTTON_B_PIN 6
#define RGB_RED_PIN 13
#define GREEN_PIN 11
#define BLUE_PIN 12
#define WS2812_PIN 7

#define NUM_PIXELS 25  // Número de LEDs na matriz 5x5
volatile int current_number = 0;

// Variáveis globais para controle do número exibido
volatile bool button_a_pressed = false;
volatile bool button_b_pressed = false;

bool green_led_state = false;  // Inicializar como desligado
bool blue_led_state = false;   // Inicializar como desligado

PIO pio = pio0;
uint sm_ws2812;
uint offset_ws2812;
ssd1306_t ssd;


// Representações binárias dos números para os LEDs
const uint32_t numbers[10][NUM_PIXELS] = {
    {0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0}, // 0
    {0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0}, // 1
    {0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0}, // 2 
    {0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0}, // 3
    {0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0}, // 4
    {0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0}, // 5
    {0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0}, // 6
    {0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0}, // 7
    {0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0}, // 8
    {0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0},  // 9
};

// Função para exibir um caractere recebido via UART no display e na matriz de LEDs
void display_number(PIO pio, uint sm, const uint32_t pattern[]) {
    for (int i = 0; i < NUM_PIXELS; i++) {
        uint32_t color = pattern[i] ? 0xFF0000 : 0x000000; // Acende ou apaga o LED
        pio_sm_put_blocking(pio, sm, color); // Envia a cor para o LED
    }
}

// Função para inicializar o WS2812
void ws2812_init() {
    offset_ws2812 = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm_ws2812, offset_ws2812, WS2812_PIN, 800000, false);
}

// Função para exibir o texto em letras maiúsculas no display
void display_uppercase_string(ssd1306_t *ssd, const char *str, uint8_t x, uint8_t y) {
    char uppercase_str[50];
    int i = 0;
    while (str[i] != '\0') {
        uppercase_str[i] = toupper(str[i]);
        i++;
    }
    uppercase_str[i] = '\0'; // Termina a string
    ssd1306_draw_string(ssd, uppercase_str, x, y);
}

// Função de debounce para os botões
void debounce_and_update(uint gpio, uint32_t events) {
    static absolute_time_t last_time = {0};
    
    if (absolute_time_diff_us(last_time, get_absolute_time()) > 200000) {
        if (gpio == BUTTON_A_PIN) {
            // Alterar o estado do LED verde
            green_led_state = !green_led_state;
            gpio_put(GREEN_PIN, green_led_state); // Ativa/desativa o LED verde

            // Exibe a informação no display
            ssd1306_fill(&ssd, false); // Limpa o display
            display_uppercase_string(&ssd, green_led_state ? "LED VERDE = On" : "LED VERDE DESLIGADO", 10, 20);
            ssd1306_send_data(&ssd);
            printf("LED Verde %s\n", green_led_state ? "ON" : "OFF");
        } else if (gpio == BUTTON_B_PIN) {
            // Alterar o estado do LED azul
            blue_led_state = !blue_led_state;
            gpio_put(BLUE_PIN, blue_led_state); // Ativa/desativa o LED azul

            // Exibe a informação no display
            ssd1306_fill(&ssd, false); // Limpa o display
            display_uppercase_string(&ssd, blue_led_state ? "LED AZUL = On" : "LED AZUL DESLIGADO", 10, 20);
            ssd1306_send_data(&ssd);
            printf("LED Azul %s\n", blue_led_state ? "ON" : "OFF");
        }
        last_time = get_absolute_time(); // Atualiza o tempo
    }
}

int main()
{
    stdio_init_all(); // Inicializa a comunicação serial

    // Inicializa I2C para o display
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ssd1306_init(&ssd, 128, 64, false, ENDERECO, I2C_PORT); // Inicializa o display
    ssd1306_config(&ssd); // Configura o display
    ssd1306_send_data(&ssd); // Envia os dados para o display

    // Inicializa o WS2812
    ws2812_init();

    // Configura botões A e B
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);
    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true, debounce_and_update);

    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_B_PIN);
    gpio_set_irq_enabled_with_callback(BUTTON_B_PIN, GPIO_IRQ_EDGE_FALL, true, debounce_and_update);

    // Configuração dos LEDs RGB (vermelho, verde e azul)
    gpio_init(RGB_RED_PIN);
    gpio_set_dir(RGB_RED_PIN, GPIO_OUT);
    gpio_put(RGB_RED_PIN, 0); // Inicializa o LED vermelho apagado

    gpio_init(GREEN_PIN);
    gpio_set_dir(GREEN_PIN, GPIO_OUT);
    gpio_put(GREEN_PIN, 0); // Inicializa o LED verde apagado

    gpio_init(BLUE_PIN);
    gpio_set_dir(BLUE_PIN, GPIO_OUT);
    gpio_put(BLUE_PIN, 0); // Inicializa o LED azul apagado

    // Loop principal
    while (true) {
        // Leitura de caractere via UART
        if (stdio_usb_connected()) {
            char c = getchar();
            if (c != EOF) {
                if (c >= '0' && c <= '9') {
                    current_number = c - '0';  // Atualiza o número para o digitado
                    display_number(pio, sm_ws2812, numbers[current_number]);  // Exibe o número na matriz WS2812
                    ssd1306_fill(&ssd, false);
                    ssd1306_draw_char(&ssd, c, 8, 10); // Exibe o caractere no display
                    ssd1306_send_data(&ssd);
                    printf("Número %c\n", c); // Exibe no Serial Monitor
                }
            }
        }
        sleep_ms(100);  // Delay para evitar sobrecarga de processamento
    }
}
