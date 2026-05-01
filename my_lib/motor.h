/**
 * File : motor.h
 * Name: Axel Fouet
 * Student nbr : 6544843
 */
#ifndef MOTOR_H
#define MOTOR_H

#include <stdint.h>
#include <stdbool.h>

#define A1A 19   // PWM left motor
#define A1B 18   // dir left
#define B1A 21   // PWM right motor
#define B1B 20   // dir right

#define PWM_SLICE_A1A 1
#define PWM_SLICE_B1A 2

#define PWM_MAX 1000

void motor_setup(void);
void motor_stop(void);

void turn_left(int percent);
void turn_right(int percent);

void ms_delay(int ms);

#endif
