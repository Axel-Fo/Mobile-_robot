/**
 * File : speed_sensor.c
 * Name: Axel Fouet
 * Student nbr : 6544843
 */
#include "speed_sensor.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/sync.h"
#include <stdio.h>


static volatile uint32_t pulse_left  = 0;
static volatile uint32_t pulse_right = 0;

//when there is a change of state HIGHT to LOW or LOW to HIGHT
static void gpio_irq_callback(uint gpio, uint32_t events) {
    if (gpio == SPEED_LEFT_PIN) {
        pulse_left++;
    }
    else if (gpio == SPEED_RIGHT_PIN) {
        pulse_right++;
    }
    printf("pulse");
}

void speed_sensor_setup(void) {

    gpio_init(SPEED_LEFT_PIN);
    gpio_set_dir(SPEED_LEFT_PIN, GPIO_IN);
    gpio_pull_up(SPEED_LEFT_PIN);

    gpio_init(SPEED_RIGHT_PIN);
    gpio_set_dir(SPEED_RIGHT_PIN, GPIO_IN);
    gpio_pull_up(SPEED_RIGHT_PIN);

    // Callback unique pour les deux GPIO
    gpio_set_irq_enabled_with_callback(
        SPEED_LEFT_PIN,
        GPIO_IRQ_EDGE_RISE,
        true,
        &gpio_irq_callback
    );

    gpio_set_irq_enabled(
        SPEED_RIGHT_PIN,
        GPIO_IRQ_EDGE_RISE,
        true
    );
}

uint32_t speed_sensor_get_left_count(void) {
    uint32_t val;
    uint32_t irq = save_and_disable_interrupts();
    val = pulse_left;
    restore_interrupts(irq);
    return val;
}

uint32_t speed_sensor_get_right_count(void) {
    uint32_t val;
    uint32_t irq = save_and_disable_interrupts();
    val = pulse_right;
    restore_interrupts(irq);
    return val;
}

// reste
void speed_sensor_reset_counts(void) {
    uint32_t irq = save_and_disable_interrupts();
    pulse_left  = 0;
    pulse_right = 0;
    restore_interrupts(irq);
}