/**
 * File : speed_sensor.h
 * Name: Axel Fouet
 * Student nbr : 6544843
 */
#ifndef SPEED_SENSOR_H
#define SPEED_SENSOR_H

#include <stdint.h>

// pins
#define SPEED_LEFT_PIN   2
#define SPEED_RIGHT_PIN  3

void speed_sensor_setup(void);

uint32_t speed_sensor_get_left_count(void);
uint32_t speed_sensor_get_right_count(void);

// Reset
void speed_sensor_reset_counts(void);

#endif
