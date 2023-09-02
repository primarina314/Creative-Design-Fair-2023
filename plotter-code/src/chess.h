#ifndef CHESS_H
#define CHESS_H

#include <Arduino.h>
#include "helpers.h"
#include "path_parser.h"

// #define FULL_SPACE 100
// #define HALF_SPACE 50

#define FULL_SPACE_X 210
#define FULL_SPACE_Y 196

#define HALF_SPACE_X 105
#define HALF_SPACE_Y 98

#define HOME_X 4
#define HOME_Y 4

#define RED 'r'
#define GREEN 'g'

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

extern int num_taken_red;
extern int num_taken_green;

extern int current_x;
extern int current_y;

extern char color;

extern boolean board[10][9];

extern int obstacles[19][17];
extern int dp[19][17];
extern int prev_dir[19][17];

void move_pen(int x_from, int y_from, int x_to, int y_to);
void move_piece(int x_from, int y_from, int x_to, int y_to);
void makeCommandOfMovePiece(int x_from, int y_from, int x_to, int y_to);

void print_board();

void updateObstaclesForMoving(int from_x, int from_y, int to_x, int to_y);
int findShortestPathWithLeastObstacles(int from_x, int from_y, int to_x, int to_y);

void onePlayerMove();
void moveForCommand();

#endif