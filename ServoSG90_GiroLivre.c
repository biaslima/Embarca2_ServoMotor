#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define SERVO_PIN 8
#define PULSE_STOP      1500 
#define PULSE_MAX_CW    1600 
#define PULSE_MAX_CCW   1400 

int main() {
    gpio_set_function(SERVO_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(SERVO_PIN);
    pwm_config config = pwm_get_default_config();
    // Frequência de 50Hz
    pwm_config_set_clkdiv(&config, 125.0f); 
    pwm_config_set_wrap(&config, 19999);    
    pwm_init(slice_num, &config, true);

    while (true) {
        pwm_set_gpio_level(SERVO_PIN, PULSE_MAX_CW);
        sleep_ms(3000);
        pwm_set_gpio_level(SERVO_PIN, PULSE_STOP);
        sleep_ms(2000);
        pwm_set_gpio_level(SERVO_PIN, PULSE_MAX_CCW);
        sleep_ms(3000);
        pwm_set_gpio_level(SERVO_PIN, PULSE_STOP);
        sleep_ms(2000);
    }
}