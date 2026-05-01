/**
 * File : avoids_object
 * Name: Axel Fouet
 * Student nbr : 6544843
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include <inttypes.h>
#include <stdbool.h>

#include "my_lib/motor.h"
#include "my_lib/hcsr04.h"
#include "my_lib/irsensor.h"

//line folower
#define SPEED_PERCENT        40
#define TURN_PERCENT         60
#define TURN_PERCENT_OTHER  -60

// to detect object
#define OBSTACLE_DIST_CM     10.0f
#define EPSILON_CM           5.0f
#define NB_MEASURES          5

//FSM states
typedef enum {
    STATE_LINE_FOLLOWER,
    STATE_AVOID_TURN_LEFT,
    STATE_OBJECT_LOST,
    STATE_FOLLOW_OBJECT,
    STATE_RETURN_TO_LINE
} robot_state_t;

int main(void) {

    //setup
    stdio_init_all();
    sleep_ms(2000);

    motor_setup();
    irsensor_setup();

    HCSR04_t front_sensor = { .trig_pin = 9, .echo_pin = 8 };
    HCSR04_t side_sensor  = { .trig_pin = 7, .echo_pin = 6 };

    hcsr04_init(&front_sensor);
    hcsr04_init(&side_sensor);

    robot_state_t state = STATE_LINE_FOLLOWER; //initialize the FSM

    float target_distance = 0.0f; // the mean of the mesurment (our target)
    float last_side_distance = 0.0f; // use to find the smalest dist of the box

    //use by the STATE_OBJECT_LOST state
    float last_lost_dist = 30.0f;
    bool decreasing_phase = false;

    //main loop
    while (true) {

        float front_dist = hcsr04_get_distance_cm(&front_sensor);

        uint16_t left_ir  = irsensor_get_analog_left();
        uint16_t right_ir = irsensor_get_analog_right();

        bool left_black  = (left_ir  < LIM_VAL_BLACK);
        bool right_black = (right_ir < LIM_VAL_BLACK);

        switch (state) {

        case STATE_LINE_FOLLOWER:
            //printf("STATE_LINE_FOLLOWER\n"); // for debug

            if (front_dist <= OBSTACLE_DIST_CM && front_dist > 0) {
                motor_stop();
                sleep_ms(200);
                last_side_distance = hcsr04_get_distance_cm(&side_sensor);
                state = STATE_AVOID_TURN_LEFT;
                break;
            }
            
            //line folower
            if (left_black && right_black) {
                turn_left(SPEED_PERCENT);
                turn_right(SPEED_PERCENT);
            }
            else if (left_black && !right_black) {
                turn_left(-TURN_PERCENT / 2);
                turn_right(TURN_PERCENT);
            }
            else if (!left_black && right_black) {
                turn_left(TURN_PERCENT);
                turn_right(-TURN_PERCENT / 2);
            }
            break;

        case STATE_AVOID_TURN_LEFT: {
            //printf("STATE_AVOID_TURN_LEFT\n"); //debug

            turn_left(-50);
            turn_right(50);

            float current_dist = hcsr04_get_distance_cm(&side_sensor);


            // when the dist increase we ahve the min dist
            if (current_dist > last_side_distance && last_side_distance > 0 && current_dist < 30) {
                motor_stop();
                sleep_ms(200);
                
                //doing the mean for the target dist
                float sum = 0.0f;
                for (int i = 0; i < NB_MEASURES; i++) {
                    sum += hcsr04_get_distance_cm(&side_sensor);
                    sleep_ms(50);
                }
                target_distance = sum / NB_MEASURES;

                //printf("Distance target = %.2f cm\n", target_distance); // for debug

                state = STATE_FOLLOW_OBJECT;
            }
            
            //still looking for the min dist
            last_side_distance = current_dist;
            break;
        }

        case STATE_FOLLOW_OBJECT: {
            //printf("STATE_FOLLOW_OBJECT\n"); //debug

            if (!left_black || !right_black) {
                motor_stop();
                sleep_ms(200);
                state = STATE_RETURN_TO_LINE;
                break;
            }

            float dist = hcsr04_get_distance_cm(&side_sensor);
            float error = dist - target_distance;

            //printf("Side dist = %.2f cm\n", dist); //debug

            if (dist > 30.0f) { // the object is lost

                last_lost_dist = 30.0f;
                decreasing_phase = false;

                state = STATE_OBJECT_LOST;
                break;
            }



            // =================================================
            if (error > EPSILON_CM) {
                // trop loin → se rapprocher
                turn_left(50);
                turn_right(0);
            }
            else if (error < -EPSILON_CM) {
                // trop proche → s’éloigner
                turn_left(0);
                turn_right(50);
            }
            else {
                // bonne distance
                turn_left(50);
                turn_right(50);
            }

            break;
        }
        // =====================================================
        case STATE_OBJECT_LOST: {
            printf("STATE_OBJECT_LOST\n");

            if (!left_black || !right_black) {
                motor_stop();
                sleep_ms(200);
                state = STATE_RETURN_TO_LINE;
                break;
            }

            // rotation sur place
            turn_left(50);
            turn_right(-50);

            float dist = hcsr04_get_distance_cm(&side_sensor);

            // clamp : tout ce qui est > 60 vaut 60
            if (dist > 30.0f) {
                dist = 30.0f;
            }

            printf("Lost dist = %.2f cm\n", dist);

            // détection de phase décroissante
            if (dist < last_lost_dist && last_lost_dist  != 30. && dist != 30) { // changed
                decreasing_phase = true;
            }

            // minimum détecté : on était en décroissance et ça réaugmente
            if (decreasing_phase && dist > last_lost_dist) {
                printf("Minimum trouvé → retour STATE_FOLLOW_OBJECT\n");

                bool find = false ; 

                for (size_t i = 0; i < 3; i++)
                {
                    turn_left(50);
                    turn_right(-50);
                    sleep_ms(25);

                    left_ir  = irsensor_get_analog_left();
                    right_ir = irsensor_get_analog_right();

                    left_black  = (left_ir  < LIM_VAL_BLACK);
                    right_black = (right_ir < LIM_VAL_BLACK);

                    if (!left_black || !right_black) {
                        motor_stop();
                        sleep_ms(200);
                        state = STATE_RETURN_TO_LINE;
                        find = true;
                        break;
                    }
                }

                if (find)
                {
                    break;
                }
                
                

                motor_stop();
                sleep_ms(200);

                state = STATE_FOLLOW_OBJECT;


                for (size_t i = 0; i < 16; i++)
                {
                    turn_left(50);
                    turn_right(50);
                    sleep_ms(25);

                    left_ir  = irsensor_get_analog_left();
                    right_ir = irsensor_get_analog_right();

                    left_black  = (left_ir  < LIM_VAL_BLACK);
                    right_black = (right_ir < LIM_VAL_BLACK);

                    if (!left_black || !right_black) {
                        motor_stop();
                        sleep_ms(200);
                        state = STATE_RETURN_TO_LINE;
                        find = true;
                        break;
                    }
                }

                break;
            }

            last_lost_dist = dist;
            break;
        }

        case STATE_RETURN_TO_LINE:
            printf("STATE_RETURN_TO_LINE\n");

            turn_left(TURN_PERCENT_OTHER);
            turn_right(TURN_PERCENT);

            ms_delay(1000);
            motor_stop();
            sleep_ms(200);

            state = STATE_LINE_FOLLOWER;
            break;
        }

        sleep_ms(20);
    }
}



// #include <stdio.h>
// #include "pico/stdlib.h"
// #include <inttypes.h>
// #include <stdbool.h>
// #include "motor.h"
// #include "hcsr04.h"
// #include "irsensor.h"

// // ----------------- CONSTANTES -----------------
// #define SPEED_PERCENT      40

// #define SPEED_PERCENT_LEFT     50
// #define SPEED_PERCENT_RIGHT    50

// #define TURN_PERCENT       60
// #define TURN_PERCENT_OTHER -60

// // #define TIME_TURN_LEFT     1740
// // #define TIME_TURN_RIGHT    1490

// #define TIME_TURN_LEFT     1470
// #define TIME_TURN_RIGHT    1740

// #define OBSTACLE_DIST_CM   10.0f
// #define SIDE_CLEAR_DIST    25.0f





// // ----------------- FSM -----------------
// typedef enum {
//     STATE_LINE_FOLLOWER,
//     STATE_AVOID_TURN_LEFT,
//     STATE_AVOID_FORWARD_1,
//     STATE_AVOID_TURN_RIGHT_1,
//     STATE_AVOID_FORWARD_2_NO_OBJ,
//     STATE_AVOID_FORWARD_2_OBJ,
//     STATE_AVOID_TURN_RIGHT_2,
//     STATE_AVOID_FIND_LINE,
//     STATE_RETURN_TO_LINE
// } robot_state_t;

// // ----------------- MAIN -----------------
// int main(void) {

//     stdio_init_all();
//     sleep_ms(2000);

//     printf("=== ROBOT START ===\n");

//     motor_setup();
//     irsensor_setup();

//     HCSR04_t front_sensor = { .trig_pin = 9, .echo_pin = 8 }; // to chaneg
//     HCSR04_t side_sensor  = { .trig_pin = 7, .echo_pin = 6 };

//     hcsr04_init(&front_sensor);
//     hcsr04_init(&side_sensor);

//     robot_state_t state = STATE_LINE_FOLLOWER;

//     turn_left(80);
//     turn_right(80);

//     while (1) {

//         float front_dist = hcsr04_get_distance_cm(&front_sensor);
//         //float side_dist  = hcsr04_get_distance_cm(&side_sensor);
//         float side_dist  = 0. ;





//         uint16_t left_ir  = irsensor_get_analog_left();
//         uint16_t right_ir = irsensor_get_analog_right();

//         printf("ir val :: ---------------->%" PRIu16 "\n", right_ir);



//         bool left_black  = (left_ir  < LIM_VAL_BLACK);
//         bool right_black = (right_ir < LIM_VAL_BLACK);

//         switch (state) {

//         // =====================================================
//         case STATE_LINE_FOLLOWER:
            
//             printf("STATE_LINE_FOLLOWER \n");

//             printf("front : %f \n",front_dist);
//             printf("side : %f \n",side_dist);

//             if (front_dist <= OBSTACLE_DIST_CM && front_dist > 0) {
//                 motor_stop();
//                 sleep_ms(200);
//                 state = STATE_AVOID_TURN_LEFT;
                
//                 turn_left(-80);
//                 turn_right(80);

//                 sleep_ms(10);

//                 break;
//             }

//             if (left_black && right_black) {
//                 turn_left(SPEED_PERCENT);
//                 turn_right(SPEED_PERCENT);
//             }
//             else if (left_black && !right_black) {
//                 turn_left(-TURN_PERCENT / 2);
//                 turn_right(TURN_PERCENT);
//             }
//             else if (!left_black && right_black) {
//                 turn_left(TURN_PERCENT);
//                 turn_right(-TURN_PERCENT / 2);
//             }
//             break;

//         // =====================================================
//         case STATE_AVOID_TURN_LEFT:

//             printf("STATE_AVOID_TURN_LEFT\n");
//             printf("front : %f \n",front_dist);
//             printf("side : %f \n",side_dist);
//             // Tourne à gauche jusqu’à être perpendiculaire à l’objet


//             turn_left(TURN_PERCENT_OTHER);
//             turn_right(TURN_PERCENT);

//             ms_delay(TIME_TURN_LEFT);
//             motor_stop();
//             sleep_ms(1000);
//             state = STATE_AVOID_FORWARD_1;

//             turn_left(80);
//             turn_right(80);

//             sleep_ms(10);
            
//             break;

//         // =====================================================
//         case STATE_AVOID_FORWARD_1:
//             printf("STATE_AVOID_FORWARD_1 \n");
//             // Avance le long de l’objet

//             turn_left(SPEED_PERCENT_LEFT);
//             turn_right(SPEED_PERCENT_RIGHT);

//             side_dist  = hcsr04_get_distance_cm(&side_sensor);

//             printf("front : %f \n",front_dist);
//             printf("side : %f \n",side_dist);

//             if (side_dist >= SIDE_CLEAR_DIST) {
//                 sleep_ms(150);

//                 motor_stop();
//                 sleep_ms(1000);
//                 state = STATE_AVOID_TURN_RIGHT_1;
//                 turn_left(80);
//                 turn_right(-80);
//                 sleep_ms(10);
//             }
//             break;

//         // =====================================================
//         case STATE_AVOID_TURN_RIGHT_1:
//             printf("STATE_AVOID_TURN_RIGHT_1\n");

//             printf("front : %f \n",front_dist);
//             printf("side : %f \n",side_dist);

//             // Premier virage à droite (90°)
 

//             turn_left(TURN_PERCENT);
//             turn_right(TURN_PERCENT_OTHER);

//             ms_delay(TIME_TURN_RIGHT);   // temps à ajuster
//             motor_stop();
//             sleep_ms(100);
//             state = STATE_AVOID_FORWARD_2_NO_OBJ;

//             turn_left(80);
//             turn_right(80);

//             sleep_ms(10);

//             break;

//         // =====================================================
//         case STATE_AVOID_FORWARD_2_NO_OBJ:
//             printf("STATE_AVOID_FORWARD_2_NO_OBJ\n");

//             // Avance le long de l’objet


//             turn_left(SPEED_PERCENT);
//             turn_right(SPEED_PERCENT);

//             side_dist  = hcsr04_get_distance_cm(&side_sensor);

//             printf("front : %f \n",front_dist);
//             printf("side : %f \n",side_dist);


//             if (side_dist < SIDE_CLEAR_DIST ) {
//                 motor_stop();
//                 sleep_ms(1000);
//                 state = STATE_AVOID_FORWARD_2_OBJ;
//                 turn_left(80);
//                 turn_right(80);

//                 sleep_ms(70);
//             }
//             break;

//         // =====================================================
//         case STATE_AVOID_FORWARD_2_OBJ:
//             printf("STATE_AVOID_FORWARD_2_OBJ\n");

//             // Avance le long de l’objet



//             turn_left(SPEED_PERCENT);
//             turn_right(SPEED_PERCENT);
            
//             side_dist  = hcsr04_get_distance_cm(&side_sensor);
//             printf("front : %f \n",front_dist);
//             printf("side : %f \n",side_dist);


//             if (side_dist >= SIDE_CLEAR_DIST) {
                
//                 sleep_ms(100);
//                 motor_stop();
//                 sleep_ms(1000);
//                 state = STATE_AVOID_TURN_RIGHT_2;
//                 turn_left(80);
//                 turn_right(-80);

//                 sleep_ms(10);
//             }
//             break;





//         // =====================================================
//         case STATE_AVOID_TURN_RIGHT_2:
//             printf("STATE_AVOID_TURN_RIGHT_2");

//             // Deuxième virage à droite (90°)


//             turn_left(TURN_PERCENT);
//             turn_right(TURN_PERCENT_OTHER);

//             ms_delay(TIME_TURN_RIGHT);
//             motor_stop();
//             sleep_ms(1000);
//             state = STATE_AVOID_FIND_LINE;
//             turn_left(80);
//             turn_right(80);
//             sleep_ms(10);
//             break;

//         // =====================================================
//         case STATE_AVOID_FIND_LINE:
//             printf("STATE_AVOID_FIND_LINE");

//             // Avance jusqu’à retrouver la ligne noire


//             turn_left(SPEED_PERCENT);
//             turn_right(SPEED_PERCENT);

//             if (!left_black || !right_black) {
//                 printf("LIGNE");
//                 motor_stop();
//                 sleep_ms(1000);
//                 state = STATE_RETURN_TO_LINE;
//                 turn_left(-80);
//                 turn_right(80);
//                 sleep_ms(10);
//             }
//             break;

//         // =====================================================
//         case STATE_RETURN_TO_LINE:
//             printf("STATE_RETURN_TO_LINE");

//             // Rotation gauche pour se réaligner avec la ligne


//             turn_left(TURN_PERCENT_OTHER);
//             turn_right(TURN_PERCENT);

//             ms_delay(2*TIME_TURN_LEFT/3);
//             motor_stop();
//             sleep_ms(1000);
//             state = STATE_LINE_FOLLOWER;
//             break;
//         }

//         sleep_ms(15);
//     }
// }
