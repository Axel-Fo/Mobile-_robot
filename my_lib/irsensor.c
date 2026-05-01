/**
 * File : irsensor.c
 * Name: Axel Fouet
 * Student nbr : 6544843
 */
#include "irsensor.h"
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"

void irsensor_setup(void) {
    // Initialisation ADC
    adc_init();
    adc_gpio_init(ANALOG_LEFT);   // GP26
    adc_gpio_init(ANALOG_RIGHT);  // GP27

    // Initialisation GPIO D0
    gpio_init(DIGITAL_LEFT);
    gpio_set_dir(DIGITAL_LEFT, GPIO_IN);

    gpio_init(DIGITAL_RIGHT);
    gpio_set_dir(DIGITAL_RIGHT, GPIO_IN);
}

uint16_t irsensor_get_analog_left(void) {
    adc_select_input(0);  // GP26 ADC0
    return adc_read();
}

uint16_t irsensor_get_analog_right(void) {
    adc_select_input(1);  // GP27 ADC1
    return adc_read();
}

bool irsensor_get_digital_left(void) {
    return gpio_get(DIGITAL_LEFT);
}

bool irsensor_get_digital_right(void) {
    return gpio_get(DIGITAL_RIGHT);
}
