# Robot — Line Following & Obstacle Avoidance

Embedded C project for a mobile robot running on the Raspberry Pi Pico. Implements motor control, line following, obstacle detection, and obstacle avoidance.

**Author:** Axel Fouet 

---

## Build & run

Built with the Raspberry Pi Pico SDK. Flash the compiled binary to the Pico via USB.

---

## Drivers

| File | Description |
|---|---|
| `motor.c/h` | PWM-based motor control (speed & direction) |
| `irsensor.c/h` | Infrared sensors for line detection |
| `hcsr04.c/h` | Ultrasonic sensor for obstacle distance |
| `speed_sensor.c/h` | Wheel encoder for odometry |

---

## Behaviors

### Line following (FSM)

| State | Behavior |
|---|---|
| `STATE_NO_BLACK` | Drive straight |
| `STATE_BLACK_LEFT` | Turn right |
| `STATE_BLACK_RIGHT` | Turn left |

### Obstacle avoidance (FSM)

| State | Behavior |
|---|---|
| `STATE_LINE_FOLLOWER` | Normal line following |
| `STATE_AVOID_TURN_LEFT` | Stop and align with obstacle |
| `STATE_FOLLOW_OBJECT` | Maintain constant distance (±4 cm epsilon) |
| `STATE_OBJECT_LOST` | Rotate to relocate obstacle |
| `STATE_RETURN_TO_LINE` | Realign with line and resume |

Obstacle detected at < 10 cm (front sensor). Object lost if side sensor reads > 30 cm.

### Odometry

Distance is estimated from wheel encoder pulses:

```
D = N × (π × Dw) / P
```

where `N` = pulse count, `Dw` = wheel diameter, `P` = pulses per revolution (measured: 60).
