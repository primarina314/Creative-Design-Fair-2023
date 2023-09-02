#ifndef PATH_PARSER_H
#define PATH_PARSER_H

#include <Arduino.h>
#include "helpers.h"

void parsePath(String path);
void parseToken(String token, workItem *wItem);
String preParse();

#endif