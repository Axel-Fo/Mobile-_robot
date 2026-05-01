/**
 * File : obstacle_detection.c
 * Name: Axel Fouet
 * Student nbr : 6544843
 */
#include <stdio.h> 
#include <stdbool.h>

#include "pico/stdlib.h"
#include "my_lib/motor.h"
#include "my_lib/hcsr04.h"


int main() {

    //setup
    stdio_init_all();
    sleep_ms(2000);
    motor_setup();
    HCSR04_t front_sensor = { .trig_pin = 9, .echo_pin = 8 };
    hcsr04_init(&front_sensor);

    turn_left(80);
    turn_right(80);

    //main loop
    while (true) {

        float dist = hcsr04_get_distance_cm(&front_sensor);
        printf("dist %2.f" , dist);
        if(dist < 15. && dist != -1){ // != -1 bc when the battery is off. the sensor return -1 (no data)
            turn_left(0);
            turn_right(0);
        }
        sleep_ms(500);
    }
}
