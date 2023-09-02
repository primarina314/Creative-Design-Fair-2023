#ifndef HELPERS_H
#define HELPERS_H

#include <Arduino.h>
#include <Servo.h>

struct workItem {
  long steps;  // total number of steps for this item
  long x;      // steps in x direction
  long y;      // steps in y direction
  byte task;
};

struct args {
  char code;
  long value;
};

/*
   3200 steps = 2cm
   1600 steps = 1cm
   160 steps  = 1mm
*/
#define DEFAULT_STEPS_PER_MM 1 //160

#define DEBUG

#define enPin1 8
#define stepPin1 6
#define dirPin1 7

#define enPin2 12
#define stepPin2 5
#define dirPin2 4

// Servo
// #define SERVO_PIN PIN_A1 // Logical pin , Servo port on motor 4 of Fabscan shield, pin 55 on mega
// #define PEN_UP 10
// #define PEN_DOWN 70

// Servo with Servo.h
#define SERVO_PIN 9
#define PEN_UP 0
#define PEN_DOWN 1
extern Servo myServo;

// LED pin
#define LED_PIN PIN_A3

// Tasks
#define TASK_MOVE 1
#define TASK_PEN_UP 2
#define TASK_PEN_DOWN 3

// bit mask is   ....YYXX where x and y have swapped
// bits, as the direction setting is different for them.
#define X_LEFT_HIT 1
#define X_RIGHT_HIT 2
#define Y_RIGHT_HIT 4
#define Y_LEFT_HIT 8

extern workItem *workItems;
extern int currentItem;

#define END_MARKER { -1, -1, -1 , TASK_MOVE}

extern int stepsPerMM;
extern String command;
extern long stepCount;
extern int motor;
extern long stepsDone;
extern boolean done;
extern long e2, err;
extern boolean continuousMode;
extern boolean compensating;
extern boolean homing;
extern int tokenCount;
extern args a; // for z-Axis
extern boolean dryRun;
extern volatile boolean xHit;
extern volatile boolean yHit;
extern volatile unsigned char hitmask;
extern volatile char hitMsg;
extern boolean verbose;

extern boolean penIsUp;

extern char dirPins[];
extern char stepPins[];

extern long dx, dy;

extern String internalCommand;
extern String externalCommand;

void enableMotors();
void disableMotors();
void setDirection(workItem item);
void servoMove(int pos);
void servoMove(int servo, int pos);
void println(String text);
void printAllWorkItems(workItem *items);
void printWorkItem(workItem wItem);
// void panic();
void oneStep();
void startWork();
void compensate_move(int motor);
args optarg (String text);


#endif