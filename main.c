#include <stdlib.h>
#include "pico/stdio.h"
#include "pico/cyw43_arch.h"
#include "hardware/adc.h"

/* Choose 'C' for Celsius or 'F' for Fahrenheit. */
#define TEMPERATURE_UNITS 'C'
#define MEASUREMENT_TIMER_MS 1000

float read_onboard_temperature(const char unit) {
    
    /* 12-bit conversion, assume max value == ADC_VREF == 3.3 V */
    const float conversionFactor = 3.3f / (1 << 12);

    float adc = (float)adc_read() * conversionFactor;
    float tempC = 27.0f - (adc - 0.706f) / 0.001721f;

    if (unit == 'C') {
        return tempC;
    } else if (unit == 'F') {
        return tempC * 9 / 5 + 32;
    }

    return -1.0f;
}

bool repeating_timer_callback(struct repeating_timer *t) {
    printf("Temperatura na CPU: %.2f %c\n", read_onboard_temperature(TEMPERATURE_UNITS), TEMPERATURE_UNITS);
    return true;
}

int main() {
    stdio_init_all();
    cyw43_arch_init();

    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(4);

    struct repeating_timer timer;

    add_repeating_timer_ms(MEASUREMENT_TIMER_MS, repeating_timer_callback, NULL, &timer);

    while(true){
        tight_loop_contents();
    }

    return 0;
}