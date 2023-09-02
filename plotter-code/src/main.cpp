
#include <Arduino.h>
#include <Servo.h>
#include "helpers.h"
#include "path_parser.h"
#include "chess.h"

// 오른쪽에 오게 할 떄, 위부터 노파초빨
// 약간 오차가 발생한 경우, 버튼을 눌러 interrupt 발생 -> 위치 재설정.

char cmd;

// void rsetup() {
  
//   // 시리얼 통신 시작 (boadrate: 9600)
//   Serial.begin(9600);
//   myServo.attach(9);
// }

// void rloop() {

//   // 컴퓨터로부터 시리얼 통신이 전송되면, 한줄씩 읽어와서 cmd 변수에 입력
//   if(Serial.available()){
//     cmd = Serial.read(); 

//     if(cmd=='a'){
//       Serial.println("arduino: a");
//       servoMove(PEN_UP);
//       delay(100);
//     }
//     else if(cmd=='b'){
//       Serial.println("arduino: b");
//       delay(100);
//       servoMove(PEN_DOWN);
//     }
//   }
// }


void setup()
{
  Serial.begin(115200);

  pinMode(enPin1, OUTPUT);
  pinMode(stepPin1, OUTPUT);
  pinMode(dirPin1, OUTPUT);
  digitalWrite(enPin1, LOW);

  pinMode(enPin2, OUTPUT);
  pinMode(stepPin2, OUTPUT);
  pinMode(dirPin2, OUTPUT);
  digitalWrite(enPin2, LOW);


  digitalWrite(dirPin1, HIGH);
  //digitalWrite(enPin1, HIGH); // Disable stepper

  digitalWrite(dirPin2, HIGH);
  //digitalWrite(enPin2, HIGH); // Disable stepper


  myServo.attach(9);
  servoMove(PEN_DOWN);

  color = GREEN;
  // print_board();
  
  // Timer1.initialize(1000);
  // Timer1.attachInterrupt(oneStep);
  // stepsDone = 0;

  // Timer1.initialize(1000);
  // Timer1.attachInterrupt(func);
}

void loop()
{
  if(Serial.available() > 0)
  {
    // input format
    // r m 1 2 3 4
    // g t 1 2 3 4
    // g m 6 8 6 7
    // r m 3 0 3 1
    // g m 9 7 7 6

    // 1890 = 18 * 105
    // 1568 = 16 * 98

    externalCommand = Serial.readStringUntil('\n');
    Serial.flush();
    command = "X0";
    onePlayerMove();


    // print_board();

    // move_pen(current_x, current_y, HOME_X, HOME_Y);

  //   command = Serial.readStringUntil('\n');
  //   if(command.charAt(0) == 'U') servoMove(PEN_UP);
  //   else if(command.charAt(0) == 'D') servoMove(PEN_DOWN);
  //   else
  //   {
  //     Serial.flush();
  //     String path = preParse(); // first char of 'command'
  //     parsePath(path);
  //     startWork();
  //   }
  //   Serial.flush();
  }
}




// void loop()
// {
//   if(Serial.available() > 0)
//   {
//     command = Serial.readStringUntil('\n');
//     Serial.print("D ");
//     Serial.println(command.c_str());
//     Serial.flush();

//     // continuousMode: false at initial
//     if(continuousMode) {

//       // we read line by line from serial input and feed it to parser+worker
//       // until the line starts with -END

//       /* EXAMPLE
//       Draw a Hilbert curve.
//       C
//       X4 Y0|X0 Y4|X-4 Y0|X0 Y4|X0 Y4|X4 Y0|X0 Y-4
//       X4 Y0|X0 Y4|X4 Y0|X0 Y-4|X0 Y-4|X-4 Y0|X0 Y-4
//       -END

//        __    __
//       |  |__|  |
//       |__    __|
//        __|  |

//       ..EXAMPLE */

//       if (command.startsWith("-END")) {
//         done = true;
//         continuousMode = false;
//         dryRun = false;
//         Serial.println("D END found, continuous mode off");
//         Serial.print("D Tokens processed: ");
//         Serial.println(tokenCount, DEC);
//         Serial.flush();
//       }
//       else {
//         // we get commands on the command line - possibly with parameters
//         String path = preParse(); // first char of 'command'
//         parsePath(path);
//         startWork();
//       }
//     } // continuousMode
//     else {
//       // we get commands on the command line - possibly with parameters
//       char c = command.charAt(0); // first char of 'command'
//       switch (c) {
//         case 'E':
//           enableMotors();
//           break;
//         case 'D':
//           disableMotors();
//           break;
//         case 'Q':
//           // Emergency shutdown for manual operation
//           // panic();
//           break;
//         case 'C': {
//             continuousMode = true;
//             Serial.print("OK Continuous mode is on");
//             if (command.length() > 2) {
//               dryRun = true;
//               Serial.print(", dryRun is on ");
//             }
//             Serial.println();
//             Serial.flush();
//             break;
//           }
//         case 'H': {
//             // Home, requires interrupts and end-switches
//             println("D Homing");
//             homing = true;
//             String path = "X-300 Y-300";
//             parsePath(path);
//             startWork();
//           }
//           break;
//         case 'u': {
//             // Pen up
//             //          penServo.write(PEN_UP);
//             servoMove(SERVO_PIN, PEN_UP);
//             Serial.println("OK pen up");
//             Serial.flush();
//           }
//           break;
//         case 'd': {
//             // Pen down
//             //          penServo.write(PEN_DOWN);
//             servoMove(SERVO_PIN, PEN_DOWN);
//             Serial.println("OK pen down");
//             Serial.flush();

//           }
//           break;
//         case 'v': {
//             int val = command.substring(1).toInt();
//             servoMove(SERVO_PIN, val);
//             break;
//           }
//         case 'i':
//           //enableEndSwitches();
//           break;
//         case 'V':
//           verbose = !verbose;
//           Serial.print(F("OK  Verbose is "));
//           Serial.println(verbose ? "on" : "off");
//           Serial.flush();
//           break;
//         case 'R': 
//           dryRun = !dryRun;
//           Serial.print(F("OK DryRun is "));
//           Serial.println(dryRun ? "on" : "off");
//           Serial.flush();
//           break;
//         case 'I': {
//             //disableXInterrupts();
//             //disableYInterrupts();
//             Serial.println("Interrupts detached");
//             Serial.flush();
//             break;
//           }

//         case 'r': { // resolution: update steps per MM
//             int val = command.substring(1).toInt();
//             int tmp = stepsPerMM;
//             stepsPerMM = val;
//             Serial.print("OK set steps perMM to ");
//             Serial.print(val);
//             Serial.print(" previous value was ");
//             Serial.println(tmp);
//             Serial.flush();
//             break;
//           }
//           /*
//         case 'z': { 
//           args arg = optarg(command);
//           handleZ(arg);
//           break;
//           }
//           */
//         case 'a':
//           // Fall through intentional
//         case 'A': 
//           //handleArc(c,command);
//           break;

//         // For simple tests - X, q, w
//         case 'X'://works well. it's for test
//           enableMotors();
//           Serial.println("X starts");
//           for(int j=0;j<2000;j++)
//           {
//             digitalWrite(stepPin1,LOW);
//             digitalWrite(stepPin2,LOW);
//             delayMicroseconds(2);
//             digitalWrite(stepPin1,HIGH); //Rising step
//             digitalWrite(stepPin2,HIGH);
//             delay(1);
//           }
//           disableMotors();
//           break;
//         case 'q':
//           digitalWrite(dirPin1, HIGH);
//           digitalWrite(dirPin2, HIGH);
//           delayMicroseconds(150);
//           break;
//         case 'w':
//           digitalWrite(dirPin1, LOW);
//           digitalWrite(dirPin2, LOW);
//           delayMicroseconds(150);
//           break;
//         default: {
//             Serial.print("E Unknown command >>");
//             Serial.print(c);
//             Serial.println("<<");
//             Serial.flush();
//           }
//       } // switch
//     } // !continuousMode
//   } // if serial

//   // digitalWrite(dirPin1, HIGH);
//   // digitalWrite(dirPin2, HIGH);

//   // for(int j=0;j<200;j++) oneStep2();

//   // delay(1000);

//   // digitalWrite(dirPin1, LOW);
//   // digitalWrite(dirPin2, LOW);
//   // for(int j=0;j<200;j++) oneStep2();

//   // delay(1000);

//   // servoFunc();

//   //delay(5000);



// }

