#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "hardware/pio.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "ws2812.pio.h" // Biblioteca para controlar LEDs WS2812
#include "inc/font.h"  // Biblioteca de fontes personalizada
#include "inc/ssd1306.h" // Biblioteca para controlar o display ssd1306

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C

#define BUTTON_A_PIN 5
#define BUTTON_B_PIN 6
#define RGB_RED_PIN 11
#define RGB_GREEN_PIN 12
#define RGB_BLUE_PIN 13
#define WS2812_PIN 7

#define NUM_PIXELS 25  // Número de LEDs na matriz 5x5
volatile int current_number = 0;

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

// Variáveis globais para controle do número exibido
volatile bool button_a_pressed = false;
volatile bool button_b_pressed = false;

PIO pio = pio0;
uint sm_ws2812;
uint offset_ws2812;

ssd1306_t ssd;
bool led_verde = false;
bool led_azul = false;

// Função de debounce para os botões
void debounce_and_update(uint gpio, uint32_t events) {
    static absolute_time_t last_time = {0};
    
    if (absolute_time_diff_us(last_time, get_absolute_time()) > 200000) {
        if (gpio == BUTTON_A_PIN) {
            current_number = (current_number + 1) % 10; // Incrementa
        } else if (gpio == BUTTON_B_PIN) {
            current_number = (current_number - 1 + 10) % 10; // Decrementa
        }
        last_time = get_absolute_time(); // Atualiza o tempo
    }
}

// Função para exibir o número no display de LEDs (matriz WS2812)
void display_number(PIO pio, uint sm, const uint32_t pattern[]) {
    for (int i = 0; i < NUM_PIXELS; i++) {
        uint32_t color = pattern[i] ? 0xFF0000 : 0x000000; // Acende ou apaga o LED
        pio_sm_put_blocking(pio, sm, color); // Envia a cor para o LED
    }
}

void button_a_isr() {
    if (button_a_pressed) return;
    button_a_pressed = true;
    led_verde = !led_verde;
    gpio_put(RGB_GREEN_PIN, led_verde);

    // Atualiza display e envia para serial
    ssd1306_fill(&ssd, false);
    ssd1306_draw_string(&ssd, led_verde ? "LED Verde ON" : "LED Verde OFF", 8, 20);
    ssd1306_send_data(&ssd);
    printf("LED Verde %s\n", led_verde ? "ON" : "OFF");
}

void button_b_isr() {
    if (button_b_pressed) return;
    button_b_pressed = true;
    led_azul = !led_azul;
    gpio_put(RGB_BLUE_PIN, led_azul);

    // Atualiza display e envia para serial
    ssd1306_fill(&ssd, false);
    ssd1306_draw_string(&ssd, led_azul ? "LED Azul ON" : "LED Azul OFF", 8, 40);
    ssd1306_send_data(&ssd);
    printf("LED Azul %s\n", led_azul ? "ON" : "OFF");
}

// Função para inicializar o WS2812
void ws2812_init() {
    offset_ws2812 = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm_ws2812, offset_ws2812, WS2812_PIN, 800000, false);
}

// Função para exibir um caractere recebido via UART no display e na matriz de LEDs
void display_character(char c) {
    if (c >= '0' && c <= '9') {
        current_number = c - '0';  // Atualiza o número para o digitado
        display_number(pio, sm_ws2812, numbers[current_number]);  // Exibe o número na matriz WS2812
        ssd1306_fill(&ssd, false);
        ssd1306_draw_char(&ssd, c, 8, 10); // Exibe o caractere no display
        ssd1306_send_data(&ssd);
        printf("Número %c\n", c); // Exibe no Serial Monitor
    }
}

int main() {
    stdio_init_all();
    
    // Inicializa I2C para o display
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);

    // Inicializa o WS2812
    ws2812_init();

    // Configura botões A e B
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);
    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true, button_a_isr);

    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_B_PIN);
    gpio_set_irq_enabled_with_callback(BUTTON_B_PIN, GPIO_IRQ_EDGE_FALL, true, button_b_isr);

    while (true) {
        // Leitura de caractere via UART
        if (stdio_usb_connected()) {
            char c = getchar();
            if (c != EOF) {
                display_character(c);  // Exibe o caractere recebido
            }
        }
        sleep_ms(100);  // Delay para evitar sobrecarga de processamento
    }
}
