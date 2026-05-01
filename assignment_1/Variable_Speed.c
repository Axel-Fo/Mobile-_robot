/**
 * File : variable_speed.c
 * Name: Axel Fouet
 * Student nbr : 6544843
 */
#include "my_lib/motor.h"

int main(void) {

    //setup
    motor_setup();

    turn_left(60);
    turn_right(60);

    ms_delay(3000);

    turn_left(100);
    turn_right(100);

    for (int p = 100; p >= 0; p -= 5) { // slowdown
        turn_left(p);
        turn_right(p);
        ms_delay(350);
    }

    turn_left(0);
    turn_right(0);

    while (true) {
        __asm("nop"); // do nothing sleep
    }
}
