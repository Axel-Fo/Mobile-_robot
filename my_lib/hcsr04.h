/**
 * File : hcsr04.h
 * Name: Axel Fouet
 * Student nbr : 6544843
 */
#ifndef HCSR04_H
#define HCSR04_H

#include "pico/stdlib.h"

#define HCSR04_TIMEOUT_US 300000

typedef struct {
    uint trig_pin;
    uint echo_pin;
} HCSR04_t;

void hcsr04_init(HCSR04_t *sensor);
float hcsr04_get_distance_cm(HCSR04_t *sensor);

#endif
