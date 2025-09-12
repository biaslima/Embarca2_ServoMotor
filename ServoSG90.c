#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "pico/bootrom.h"

#define SERVO_PIN 8
#define JOYSTICK_PIN 27
#define LED_PIN_RED 13
#define LED_PIN_BLUE 12
#define LED_PIN_GREEN 11
#define BOTAO_RESET 6
#define BOTAO_MODO 5 

#define MIN_PULSE 500
#define MAX_PULSE 2500

#define ADC_INPUT 1

//GRAVAÇÃO
#define MAX_RECORD_POSITIONS 200 
uint16_t recorded_positions[MAX_RECORD_POSITIONS];
int record_index = 0;
bool is_recording = false;

//ESTADOS
typedef enum {
    MANUAL,
    SWEEP,
    RECORD_PLAYBACK
} OperatingMode;

volatile OperatingMode current_mode = MANUAL; 

// Função para mapear valores 
long map(long x, long in_min, long in_max, long out_min, long out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

//INTERRUPÇÃO 
// Interrupção para o botão de RESET
void gpio_irq_handler_reset(uint gpio, uint32_t events) {
    reset_usb_boot(0, 0);
}

// Interrupção do botão de MODO
void gpio_irq_handler_mode(uint gpio, uint32_t events) {
    current_mode = (current_mode + 1) % 3;

    // Reset gravação se entrar em modo de gravação
    if (current_mode == RECORD_PLAYBACK) {
        record_index = 0;
        is_recording = true; 
    } else {
        is_recording = false;
    }
}

int main() {
    gpio_init(BOTAO_RESET);
    gpio_set_dir(BOTAO_RESET, GPIO_IN);
    gpio_pull_up(BOTAO_RESET);
    gpio_set_irq_enabled_with_callback(BOTAO_RESET, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler_reset);

    gpio_init(BOTAO_MODO);
    gpio_set_dir(BOTAO_MODO, GPIO_IN);
    gpio_pull_up(BOTAO_MODO);
    gpio_set_irq_enabled_with_callback(BOTAO_MODO, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler_mode);


    gpio_init(LED_PIN_RED);
    gpio_set_dir(LED_PIN_RED, GPIO_OUT);
    gpio_init(LED_PIN_BLUE);
    gpio_set_dir(LED_PIN_BLUE, GPIO_OUT);
    gpio_init(LED_PIN_GREEN);
    gpio_set_dir(LED_PIN_GREEN, GPIO_OUT);

    
    adc_init();
    adc_gpio_init(JOYSTICK_PIN);
    adc_select_input(ADC_INPUT);

    // PWM para servo
    uint slice_num = pwm_gpio_to_slice_num(SERVO_PIN);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 125.0f);
    pwm_config_set_wrap(&config, 19999);
    pwm_init(slice_num, &config, true);
    gpio_set_function(SERVO_PIN, GPIO_FUNC_PWM);

    // Variáveis SWEEP
    int sweep_pulse = MIN_PULSE;
    int sweep_step = 10;

    while (true) {
        uint16_t joystick_value = adc_read();

        switch (current_mode) {
            case MANUAL:
                gpio_put(LED_PIN_BLUE, 1); 
                gpio_put(LED_PIN_GREEN, 0); 
                gpio_put(LED_PIN_RED, 0); 
                
                int pulse = map(joystick_value, 0, 4095, MIN_PULSE, MAX_PULSE);
                pwm_set_gpio_level(SERVO_PIN, pulse);
                sleep_ms(20);
                break;

            case SWEEP:
                gpio_put(LED_PIN_BLUE, 0); 
                gpio_put(LED_PIN_GREEN, 1); 
                gpio_put(LED_PIN_RED, 0); 
                
                sweep_pulse += sweep_step;
                if (sweep_pulse >= MAX_PULSE || sweep_pulse <= MIN_PULSE) {
                    sweep_step = -sweep_step; 
                }
                pwm_set_gpio_level(SERVO_PIN, sweep_pulse);
                
                // Joystick controla a velocidade 
                int delay_time = map(joystick_value, 0, 4095, 30, 1); 
                sleep_ms(delay_time);
                break;

            case RECORD_PLAYBACK:
                gpio_put(LED_PIN_BLUE, 0);
                gpio_put(LED_PIN_GREEN, 0);

                if (is_recording) {
                    // LED vermelho pisca
                    gpio_put(LED_PIN_RED, !gpio_get(LED_PIN_RED));
                    
                    if (record_index < MAX_RECORD_POSITIONS) {
                        recorded_positions[record_index] = joystick_value;
                        int rec_pulse = map(joystick_value, 0, 4095, MIN_PULSE, MAX_PULSE);
                        pwm_set_gpio_level(SERVO_PIN, rec_pulse);
                        record_index++;
                    } else {
                        is_recording = false;
                        gpio_put(LED_PIN_RED, 1);
                    }
                } else {
                    // LED vermelho aceso
                    gpio_put(LED_PIN_RED, 1);
                    
                    for (int i = 0; i < record_index; i++) {
                        int play_pulse = map(recorded_positions[i], 0, 4095, MIN_PULSE, MAX_PULSE);
                        pwm_set_gpio_level(SERVO_PIN, play_pulse);
                        sleep_ms(30);
                    }
                }
                sleep_ms(30);
                break;
        }
    }
}
