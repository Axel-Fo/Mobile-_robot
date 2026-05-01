/**
 * File : irsensor.h
 * Name: Axel Fouet
 * Student nbr : 6544843
 */
#ifndef IRSENSOR_H
#define IRSENSOR_H

#include <stdint.h>
#include <stdbool.h>

#define ANALOG_RIGHT 26
#define DIGITAL_RIGHT 16

#define ANALOG_LEFT 27
#define DIGITAL_LEFT 17


#define LIM_VAL_BLACK 1000

void irsensor_setup(void);
uint16_t irsensor_get_analog_left(void);
uint16_t irsensor_get_analog_right(void);
bool irsensor_get_digital_left(void);
bool irsensor_get_digital_right(void);

#endif
