/**
 * File : motor.c
 * Name: Axel Fouet
 * Student nbr : 6544843
 */
#include "motor.h"

#include "hardware/regs/io_bank0.h"
#include "hardware/regs/pwm.h"
#include "hardware/structs/io_bank0.h"
#include "hardware/structs/pwm.h"
#include "hardware/structs/sio.h"

#define MASK(x) (1u << (x))

void ms_delay(int ms) {
    while (ms-- > 0) {
        volatile int x = 6000;
        while (x-- > 0)
            __asm("nop");
    }
}

// GPIO INIT
static void gpio_init_output(uint gpio) {
    io_bank0_hw->io[gpio].ctrl =
        GPIO_FUNC_SIO << IO_BANK0_GPIO0_CTRL_FUNCSEL_LSB;
    sio_hw->gpio_oe_set = MASK(gpio);
}

static void gpio_init_pwm(uint gpio) {
    io_bank0_hw->io[gpio].ctrl =
        GPIO_FUNC_PWM << IO_BANK0_GPIO0_CTRL_FUNCSEL_LSB;
}

// PWM
static void pwm_set_duty(uint slice, uint channel, uint16_t duty) {
    if (duty > PWM_MAX) duty = PWM_MAX;

    if (channel == 0) {
        pwm_hw->slice[slice].cc =
            (pwm_hw->slice[slice].cc & 0xFFFF0000) | duty;
    } else {
        pwm_hw->slice[slice].cc =
            (pwm_hw->slice[slice].cc & 0x0000FFFF) | (duty << 16);
    }
}

static int percent_to_duty(int percent) {
    if (percent > 100) percent = 100;
    if (percent < -100) percent = -100;
    return (percent * PWM_MAX) / 100;
}


void motor_stop(void) {
    turn_right(0);
    turn_left(0);
}


void motor_setup(void) {

    gpio_init_output(A1B);
    gpio_init_output(B1B);

    gpio_init_pwm(A1A);
    gpio_init_pwm(B1A);

    pwm_hw->slice[PWM_SLICE_A1A].top = PWM_MAX;
    pwm_hw->slice[PWM_SLICE_A1A].csr |= PWM_CH0_CSR_EN_BITS;

    pwm_hw->slice[PWM_SLICE_B1A].top = PWM_MAX;
    pwm_hw->slice[PWM_SLICE_B1A].csr |= PWM_CH0_CSR_EN_BITS;

    sio_hw->gpio_clr = MASK(A1B) | MASK(B1B);

    motor_stop();
}

//to make the right wheel turn
void turn_right(int percent) {
    int duty = percent_to_duty(percent);
    bool reverse = (duty < 0);
    int speed = reverse ? -duty : duty;

    if (reverse) {
        sio_hw->gpio_set = MASK(A1B);
    } else {
        sio_hw->gpio_clr = MASK(A1B);
    }

    pwm_set_duty(PWM_SLICE_A1A, 1, speed);
}

//to make the left wheel turn
void turn_left(int percent) {
    int duty = percent_to_duty(percent);
    bool reverse = (duty < 0);
    int speed = reverse ? -duty : duty;

    if (reverse) {
        sio_hw->gpio_set = MASK(B1B);
    } else {
        sio_hw->gpio_clr = MASK(B1B);
    }

    pwm_set_duty(PWM_SLICE_B1A, 1, speed);
}
