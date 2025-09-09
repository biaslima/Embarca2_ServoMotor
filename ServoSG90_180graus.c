#include "pico/stdlib.h"
#include "hardware/pwm.h"


#define SERVO_PIN 8
#define MIN_PULSE 500  // Pulso para ~0 graus
#define MAX_PULSE 2500 // Pulso para ~180 graus
#define led_pin_red 13
#define GPIO_LIGA 1
#define GPIO_DESLIGA 0
#define DELAY_PER_DEGREE 50 

#include "pico/bootrom.h"
#define botaoB 6
void gpio_irq_handler(uint gpio, uint32_t events) {
    reset_usb_boot(0, 0);
}

long map(long x, long in_min, long in_max, long out_min, long out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int main() {
    gpio_init(botaoB);
    gpio_set_dir(botaoB, GPIO_IN);
    gpio_pull_up(botaoB);
    gpio_set_irq_enabled_with_callback(botaoB, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    gpio_init(led_pin_red);
    gpio_set_dir(led_pin_red, GPIO_OUT);

    gpio_set_function(SERVO_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(SERVO_PIN);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 125.0f);
    pwm_config_set_wrap(&config, 19999);
    pwm_init(slice_num, &config, true);

     while (true) {
        gpio_put(led_pin_red, GPIO_LIGA);

        for (int angle = 0; angle <= 180; angle++) {
            int pulse = map(angle, 0, 180, MIN_PULSE, MAX_PULSE);
            pwm_set_gpio_level(SERVO_PIN, pulse);
            sleep_ms(DELAY_PER_DEGREE); 
        }
        gpio_put(led_pin_red, GPIO_DESLIGA);
        sleep_ms(500); 
        gpio_put(led_pin_red, GPIO_LIGA);

        for (int angle = 180; angle >= 0; angle--) {
            int pulse = map(angle, 0, 180, MIN_PULSE, MAX_PULSE);
            pwm_set_gpio_level(SERVO_PIN, pulse);
            sleep_ms(DELAY_PER_DEGREE); 
        }
        gpio_put(led_pin_red, GPIO_DESLIGA);
        sleep_ms(500); 
    }
}