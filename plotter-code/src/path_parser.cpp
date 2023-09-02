
#include <Arduino.h>
#include "helpers.h"

void parsePath(String path);
void parseToken(String token, workItem *wItem);
String preParse();


/* Parse a path that is in the form of
    Xnn
    Ynn
    Xnn Ynn
    where items are separated by pipe ( | ).
    nn is a relative distance in millimeter
    E.g. the following to build a triangle
   "X50|Y50|X-50 Y-50";

   X30 - move 30 units on the X-axis
   Y-2 - move -2 units on the Y-axis
   X10 Y5 - move 10 units on the X-axis and also 5 units on the y-axis
   X2 Y2|X-2 - first move 2 units on the X-axis and 2 on the y-axis. Then move -2 units on the X-axis
   
   The units depend on the setting of the plotter. In my case 1 unit = 1mm.
*/
void parsePath(String path) {

#ifdef DEBUG
    // Serial.print("Parse Path input ");
    // Serial.println(path);
    // Serial.flush();
    long t1 = millis();
#endif
    path.trim();// erase blank at both(maybe) ends
    String sub = path;

    int segments = 0;
    int curPos = 0;
    int pos;

    // count segments separated by |
    do {
        pos = path.indexOf('|', curPos);
        segments++;
        curPos = pos + 1;
    } while (pos > 0);

    // Serial.print("D Segments: ");
    // Serial.println( segments, DEC);

    // Now parse the segments and create work items
    curPos = 0;
    int itemCount = 0;
    if (segments > 0) {
        do {
            String token;
            pos = path.indexOf('|', curPos);
            if (pos != -1) {
                // X50 Y30|X20 Y10 -> token: 'X50 Y30'
                token = sub.substring(curPos, pos);
            } else {
                // last token
                token = sub.substring(curPos);
            }
#ifdef DEBUG
            // Serial.print("D Found token >>");
            // Serial.print( token.c_str());
            // Serial.println("<<");
            // Serial.flush();
#endif
            if (token.startsWith("a") || token.startsWith("A") ) {
                // handleArc(token);
                return; // we get this as single token, so we are done here.
            } else {
                // save the informations of token to workItem
                parseToken(token, &workItems[itemCount]);
                itemCount++;
            }
            tokenCount++;
            curPos = pos + 1; // skip over |
        } while (pos > 0);
    }

    workItems[itemCount] = END_MARKER;//last item

#ifdef DEBUG
    // long t2 = millis();
    // Serial.print("D Parsing took " );
    // Serial.print(t2 - t1, DEC);
    // Serial.println(" ms");
    // Serial.flush();
#endif

}

/*
 * Parse a single token of the path and put it into the passed
 * workItem
 */
void parseToken(String token, workItem *wItem)
{
    // Tokens are separated by '|'
    // e.g. X40 Y-20|X30|X40 Y20 -> 3 tokens: X40 Y-20, X30, X40 Y20

    long x = 0;
    long y = 0;

    int pos ;
    int curPos = 0;

#ifdef DEBUG
    unsigned long t1 = micros();
#endif

    do {
        char v = token.charAt(curPos);

        curPos++;
        long val;
        pos = token.indexOf(' ', curPos);
        if (pos > 0) val = token.substring(curPos, pos).toInt();
        else val = token.substring(curPos).toInt();// value of movement. 50 of X50


        // item contain x=val and y=val as relative loc of next target
        if (v == 'X') {
            x = val;
            wItem->task = TASK_MOVE;
        } else if (v == 'Y') {
            y = val;
            wItem->task = TASK_MOVE;
        } else if (v == 'U') {
            wItem->task = TASK_PEN_UP;
        } else if (v == 'D') {
            wItem->task = TASK_PEN_DOWN;
        } else {
            // Serial.print(F("E  unknown code "));
            // Serial.println(v);
            // Serial.flush();
        }
        curPos = pos + 1;
    } while (pos > 0);// at most 2 times repeat

    wItem->x = x * stepsPerMM;
    wItem->y = y * stepsPerMM;

    wItem->steps = max(abs(wItem->x), abs(wItem->y));

    // printWorkItem(*wItem);

#ifdef DEBUG
    unsigned long t2 = micros();

    // Serial.print(F("D     parseToken: (us) "));
    // Serial.println(t2 - t1, DEC);
    // Serial.println();
    // Serial.flush();
#endif

    // if (verbose) {
    //     printWorkItem(*wItem);
    // }
}


String preParse()
{
    // Scan command
    // command -> path
    int itemCount = 0;
    
    for (unsigned int i = 0; i < command.length(); i++) {
        if (command.charAt(i) == '|') {
            itemCount++;
        }
    }

    // Serial.print("D preParse: itemCount: ");
    // Serial.println(itemCount, DEC);
    // Serial.flush();
    // We did not hit 10, so return the whole string's end
    return command;
}
