/**
 * File : fixed_distance.c
 * Name: Axel Fouet
 * Student nbr : 6544843
 */
#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include <inttypes.h>

#include "my_lib/motor.h"
#include "my_lib/irsensor.h"
#include "my_lib/speed_sensor.h"

//param
#define TARGET_DISTANCE_M  1.3f

#define WHEEL_DIAMETER_M       0.065f 
#define WHEEL_CIRCUMFERENCE   (3.1415926f * WHEEL_DIAMETER_M)
#define ENCODER_PULSES_PER_REV 60 // obtained by testing

#define TARGET_PULSES ((uint32_t)((TARGET_DISTANCE_M / WHEEL_CIRCUMFERENCE) * ENCODER_PULSES_PER_REV))

//for line folower
#define SPEED_PERCENT 40
#define TURN_PERCENT  60

int main(void) {

    stdio_init_all();
    sleep_ms(2000);

    //printf("TARGET_PULSES : = %lu\n", TARGET_PULSES); //debug

    //setup
    motor_setup();
    irsensor_setup();
    speed_sensor_setup();

    // Reset count
    speed_sensor_reset_counts();

    //main loop
    while (true) {

        uint32_t left_count  = speed_sensor_get_left_count();
        uint32_t right_count = speed_sensor_get_right_count();

        uint32_t min_count = (left_count < right_count) ? left_count : right_count; // we take the smalest val to reduce imprecision
        
        //printf("count = %" PRIu32 "\n", min_count); //debug


        if (min_count >= TARGET_PULSES) { // dist reatched
            break;
        }

        uint16_t left_val  = irsensor_get_analog_left();
        uint16_t right_val = irsensor_get_analog_right();

        bool left_black  = (left_val  < LIM_VAL_BLACK);
        bool right_black = (right_val < LIM_VAL_BLACK);

        // line folower as A3
        if (left_black && right_black) {
            turn_left(SPEED_PERCENT);
            turn_right(SPEED_PERCENT);
        }
        else if (left_black && !right_black) {
            turn_left(-TURN_PERCENT/2);
            turn_right(TURN_PERCENT);
        }
        else if (!left_black && right_black) {
            turn_left(TURN_PERCENT);
            turn_right(-TURN_PERCENT/2);
        }

        
        //printf("pulses L=%lu R=%lu mean=%lu | IR L=%u R=%u\n",left_count, right_count, min_count,left_val, right_val); //debugf

        sleep_ms(15);
    }

    turn_left(0);
    turn_right(0);

    while (1) {
        tight_loop_contents(); // wait
    }
}




