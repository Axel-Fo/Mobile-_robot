/**
 * File : line_folower.c
 * Name: Axel Fouet
 * Student nbr : 6544843
 */
#include <stdio.h>
#include <inttypes.h>
#include "pico/stdlib.h"
#include "my_lib/irsensor.h"
#include "my_lib/motor.h"

#define SPEED_PERCENT 50

//diff val for balance
#define TURN_PERCENT_RIGHT  60
#define OPPOSIT_TURN_PERCENT_RIGHT  -30

#define TURN_PERCENT_LEFT  65
#define OPPOSIT_TURN_PERCENT_LEFT  -25

#define TIME_STATE_MS 10


typedef enum {
    STATE_NO_BLACk,
    STATE_BLACK_LEFT,
    STATE_BLACK_RIGHT,

} robot_state_t;



int main(void) {
    
    //setup
    stdio_init_all();
    sleep_ms(2000);

    motor_setup();
    irsensor_setup();

    robot_state_t state = STATE_NO_BLACk; //initialize the FSM

    //main loop
    while(true) {

                
        uint16_t left_val  = irsensor_get_analog_left();
        uint16_t right_val = irsensor_get_analog_right();

        bool left_black  = (left_val  > LIM_VAL_BLACK);
        bool right_black = (right_val > LIM_VAL_BLACK);

        switch (state)
        {
        case STATE_NO_BLACk:

            turn_left(SPEED_PERCENT);
            turn_right(SPEED_PERCENT);
            sleep_ms(TIME_STATE_MS);
            // always turn off the motor to slow down
            turn_left(0);
            turn_right(0);

            if (!left_black && right_black) {

                state = STATE_BLACK_RIGHT;
            }

            if (left_black && !right_black){

                state = STATE_BLACK_LEFT;
            }

            break;

        case STATE_BLACK_LEFT:

            turn_left(OPPOSIT_TURN_PERCENT_LEFT);
            turn_right(TURN_PERCENT_LEFT);
            sleep_ms(TIME_STATE_MS);
            // always turn off the motor to slow down
            turn_left(0);
            turn_right(0);

            if (!right_black){
                if (left_black)
                {
                    state = STATE_BLACK_RIGHT;
                }else{
                    state = STATE_NO_BLACk;
                }

            }

            break;
        case STATE_BLACK_RIGHT:

            turn_left(TURN_PERCENT_RIGHT);
            turn_right(OPPOSIT_TURN_PERCENT_RIGHT);
            sleep_ms(TIME_STATE_MS);
            // always turn off the motor to slow down
            turn_left(0);
            turn_right(0);

            if (!left_black){
                if (right_black)
                {
                    state = STATE_BLACK_LEFT;
                }else{
                    state = STATE_NO_BLACk;
                }

            }

            break;
        
        default:
            printf("Wrong stat"); 
            break;
        }
        sleep_ms(5);
    }
}























