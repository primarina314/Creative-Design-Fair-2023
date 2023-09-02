
#include <Arduino.h>
#include <Servo.h>

/*
   3200 steps = 2cm
   1600 steps = 1cm
   160 steps  = 1mm
*/

#define DEFAULT_STEPS_PER_MM 1

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
#define PEN_ANGLE 180
Servo myServo;

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

workItem *workItems = new workItem[50];// array size 를 크게 하면 변수값이 원치않게 바뀌는 오류 발생 가능.
int currentItem = 0;

#define END_MARKER { -1, -1, -1 , TASK_MOVE}

int stepsPerMM = DEFAULT_STEPS_PER_MM;
String command;
long stepCount;
int motor;
long stepsDone = 0;
boolean done;
long e2, err;
boolean continuousMode = false;
boolean compensating = false;
boolean homing = false;
int tokenCount;
args a; // for z-Axis
boolean dryRun = false;
volatile boolean xHit = false;
volatile boolean yHit = false;
volatile unsigned char hitmask = 0x0;
volatile char hitMsg = '\0';
boolean verbose = false;

boolean penIsUp;

char dirPins[] = {7, 4};
char stepPins[] = {6, 5};

long dx, dy;

String internalCommand;
String externalCommand;

void oneStep();

void enableMotors();
void disableMotors();
void setDirection(workItem item);
void servoMove(int servo, int pos);
void println(String text);


void printAllWorkItems(workItem *items);
void printWorkItem(workItem wItem);

void panic();
void oneStep();


void startWork();

void compensate_move(int motor);
args optarg (String text);


void enableMotors()
{
  digitalWrite(enPin1, LOW);
  digitalWrite(enPin2, LOW);
  // Just 'enable' state, not rotating
}

void disableMotors()
{
  digitalWrite(enPin1, HIGH);
  digitalWrite(enPin2, HIGH);
}

void setDirection(workItem item)
{
  // item contain x and y as relative loc of next target
  // set rotating dir depending on the relative location of item
  if (!dryRun) {
    if (item.x > 0) {
      digitalWrite(dirPins[0], HIGH);
    } else {
      digitalWrite(dirPins[0], LOW);
    }

    if (item.y > 0) {
      digitalWrite(dirPins[1], LOW);
    } else {
      digitalWrite(dirPins[1], HIGH);
    }

    delayMicroseconds(150);
  }
}

void servoMove(int pos)
{
  if (dryRun) {
    return;
  }
  
  if(pos == PEN_UP) myServo.write(114);
  else if(pos == PEN_DOWN) myServo.write(0);

  penIsUp = (pos==PEN_UP);
  delay(300);
  
  return;

  if(pos==PEN_UP) {
    for(int ang=0;ang<=PEN_ANGLE;ang++)
    {
      myServo.write(ang);
      delay(20);
    }
  }
  else if(pos==PEN_DOWN) {
    for(int ang=PEN_ANGLE;ang>=0;ang--)
    {
      myServo.write(ang);
      delay(20);
    }
  }
  penIsUp = (pos==PEN_UP);
  delay(1000);
  return;
}

void servoMove(int servo, int pos)
{
  // called by - servoMove(SERVO_PIN, PEN_UP)
  // PEN_UP: 10
  // PEN_DOWN: 70
  if (dryRun) {
    return;
  }
  
  int pwm;
  // Create millis for HIGH duration
  pwm = (pos * 11) + 544;
  // PEN_UP: 770+544 = 1314
  // PEN_DOWN: 110+544 = 654

  // Iterate a few times to give the servo a chance
  // for its arm to move
  for (int i = 0; i < 35 ; i++) {
    // One signal round is 20ms
    digitalWrite(servo, HIGH);
    delayMicroseconds(pwm);
    digitalWrite(servo, LOW);
    delayMicroseconds(20000);//20ms
  }
  penIsUp = (pos == PEN_UP);
  // 서보모터 각도를 기반으로 움직이는 방식 고려
  // https://wikidocs.net/30882
  // Servo.h 의 write(angle)
  
}

void println(String text) {
#ifdef DEBUG  
  Serial.println(text);
  Serial.flush();
#endif  
}


void printAllWorkItems(workItem *items) {
#ifdef DEBUG
  int count = 0;
  boolean done = false;
  do {
    workItem item = items[count];
    printWorkItem(item);
    count++;
    if (item.steps < 0 ) {
      done = true;
    }
  } while (!done);

#endif
}

void printWorkItem(workItem wItem) {
#ifdef DEBUG
  Serial.print("D workItem{steps=");
  Serial.print(wItem.steps, DEC);
  Serial.print(", x=");
  Serial.print(wItem.x, DEC);
  Serial.print(", y=");
  Serial.print(wItem.y, DEC);
  Serial.print(", task=");
  Serial.print(wItem.task);
  Serial.println("}");

  Serial.flush();
#endif
}

// void panic() {
//   Timer1.stop();
//   println("STOPPED");
//   done = true;
//   xHit = true;
//   yHit = true;
//   continuousMode = false;
// }


void oneStep() {
  boolean doX = false, doY = false;

  // if (xHit || yHit)  {
  //   println(F("D One step, hit a switch"));
  //   servoMove(SERVO_PIN, PEN_UP);
  //   Timer1.stop();
  //   Timer1.detachInterrupt();
  //   // delete[] workItems;
  //   done = true;
  //   return;
  // }

  workItem wItem = workItems[currentItem];

  if (wItem.task == TASK_PEN_UP) {
    servoMove(PEN_UP);
    currentItem++;
    return;
  }
  if (wItem.task == TASK_PEN_DOWN) {
    servoMove(PEN_DOWN);
    currentItem++;
    return;
  }

  //X0 Y0 or X0 or Y0
  if(wItem.steps == 0)
  {
    currentItem++;
    return;
  }

  // From here on:  TASK_MOVE

  // wItem.steps <= 0 means END_MARKER, so we can stop with this
  // set of work items
  // 'steps' of END_MARKER is -1.
  if (wItem.steps <= 0) {
    done = true;
    return;
  }

  // No end maker? Then let's move

  // at init of wItem
  if (stepsDone == 0) {
    // if (verbose) {
    //   printWorkItem(wItem);
    // }
    setDirection(wItem);
    dx = abs(wItem.x);
    dy = -abs(wItem.y);
    err = dx+dy;
  }

/*
30 -10 

err 20, e2 40 -> X
err 10, e2 20 -> X, Y
err 30, e2 60 -> X
repeat
err 20, e2 40 -> X


*/

  if (!dryRun) {
    // Bessenham algorithm from wikipedia
    // Draw aprroxiated linear line

    e2 = 2 * err;
    if (e2 > dy) {
      digitalWrite(stepPins[0], HIGH);
      err += dy;
      doX = true;
    }
    if (e2 < dx) {
      digitalWrite(stepPins[1], HIGH);
      err += dx;
      doY = true;
    }
    delayMicroseconds(3000);
    if (doX) {
      digitalWrite(stepPins[0], LOW);
    }
    if (doY) {
      digitalWrite(stepPins[1], LOW);
    }
  }

  stepsDone++;
  if (stepsDone >= abs(wItem.steps)) {
    // > or >= ?
    currentItem++;
    stepsDone = 0;
  }
} //oneStep


void startWork() {
  // Serial.println("StartWork..");
  currentItem = 0;

  // xHit = yHit = false;
  stepsDone = 0;
  // if (!dryRun) {
  //   enableMotors();
  // }
  enableMotors();
  done = false;

  // Serial.println("D Starting...");
  // Serial.flush();

  while(!done)
  {
    oneStep();
    delayMicroseconds(3000);
  }

}

void compensate_move(int motor) {
  Serial.print("D motor = ");
  Serial.print(motor, DEC);
  Serial.print(" mask = ");
  Serial.print(hitmask, BIN);
  println(" Start crawling");

  unsigned char mask = hitmask;
  if (motor == 1) {
    mask >>= 2;
  }
  mask &= 3;

  if (mask & 1) {
    digitalWrite(dirPins[motor], HIGH);
  }
  else {
    digitalWrite(dirPins[motor], LOW);
  }
  delayMicroseconds(150);

  delay(1);

  // Now we move until the switch is released, which is signalled by an interrupt.
  while (!xHit && !yHit) {
    digitalWrite(stepPins[motor], HIGH);
    delayMicroseconds(30);
    digitalWrite(stepPins[motor], LOW);
    delayMicroseconds(150);
  }
  println("D End crawling");
}

args optarg (String text) {
  
  a.code = text.charAt(1);
  a.value = text.substring(2).toInt();

  return a;
}


