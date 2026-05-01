/**
 * File : hcsr04.c
 * Name: Axel Fouet
 * Student nbr : 6544843
 */
#include "hcsr04.h"
#include "hardware/gpio.h"
#include "hardware/clocks.h"
#include "hardware/pio.h"
#include "hcsr04.pio.h"

static PIO pio = pio0;
static uint sm = 0;

static void echo_pio_init(uint echo_gpio) {
    uint offset = pio_add_program(pio, &if_echo_program);

    pio_sm_config c = if_echo_program_get_default_config(offset);
    sm_config_set_in_pins(&c, echo_gpio);
    sm_config_set_jmp_pin(&c, echo_gpio);

    // PIO = 1 MHz  1 tick = 1 µs
    float div = (float)clock_get_hz(clk_sys) / 1e6f;
    sm_config_set_clkdiv(&c, div);

    pio_gpio_init(pio, echo_gpio);
    pio_sm_set_consecutive_pindirs(pio, sm, echo_gpio, 1, false);

    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);
}

void hcsr04_init(HCSR04_t *sensor) {
    // TRIG
    gpio_init(sensor->trig_pin);
    gpio_set_dir(sensor->trig_pin, GPIO_OUT);
    gpio_put(sensor->trig_pin, 0);

    // ECHO via PIO
    gpio_init(sensor->echo_pin);
    gpio_set_dir(sensor->echo_pin, GPIO_IN);
    echo_pio_init(sensor->echo_pin);
}

float hcsr04_get_distance_cm(HCSR04_t *sensor) {
    gpio_put(sensor->trig_pin, 0);
    sleep_us(2);
    gpio_put(sensor->trig_pin, 1);
    sleep_us(10);
    gpio_put(sensor->trig_pin, 0);

    uint32_t x_final;
    while (pio_sm_is_rx_fifo_empty(pio, sm)) {
        tight_loop_contents();
    }
    x_final = pio_sm_get(pio, sm);

    uint32_t ticks = 0xFFFFFFFF - x_final; //we reverse bc

    // 1 tick = 1 us, speed of sound ~ 343 m/s = 0.0343 cm/us
    float distance_cm = (ticks * 0.0343f) / 2.0f;

    return distance_cm;
}
