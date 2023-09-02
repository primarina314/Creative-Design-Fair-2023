
#include <Arduino.h>
#include "helpers.h"
#include "path_parser.h"


// #define FULL_SPACE 100
// #define HALF_SPACE 50

#define FULL_SPACE_X -210
#define FULL_SPACE_Y -196

#define HALF_SPACE_X -105
#define HALF_SPACE_Y -98

// expression must have integral or unscoped enum type

#define HOME_X 0
#define HOME_Y 0


#define RED 'r'
#define GREEN 'g'

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

#define LEN_X 19
#define LEN_Y 17

#define MAX 10000
#define OCCUPIED 2000
#define P 2000
// P: piece
int num_taken_red = 0;
int num_taken_green = 0;
int current_x = HOME_X;
int current_y = HOME_Y;

char color;
// piece class/struct

int obstacles[LEN_X][LEN_Y] = {
    {P, 2, P, 2, P, 2, P, 1, 0, 1, P, 2, P, 2, P, 2, P},
    {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
    {0, 0, 0, 0, 0, 0, 0, 1, P, 1, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0},
    {0, 1, P, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, P, 1, 0},
    {1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1},
    {P, 1, 0, 1, P, 1, 0, 1, P, 1, 0, 1, P, 1, 0, 1, P},
    {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1},
    {P, 1, 0, 1, P, 1, 0, 1, P, 1, 0, 1, P, 1, 0, 1, P},
    {1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1},
    {0, 1, P, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, P, 1, 0},
    {0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 1, P, 1, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
    {P, 2, P, 2, P, 2, P, 1, 0, 1, P, 2, P, 2, P, 2, P}
};

int dp[LEN_X][LEN_Y];
int prev_dir[LEN_X][LEN_Y];

boolean board[10][9] = {
    {1, 1, 1, 1, 0, 1, 1, 1, 1},
    {0, 0, 0, 0, 1, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0, 1, 0},
    {1, 0, 1, 0, 1, 0, 1, 0, 1},
    {0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 1, 0, 1, 0, 1, 0, 1},
    {0, 1, 0, 0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0},
    {1, 1, 1, 1, 0, 1, 1, 1, 1}
};

bool skip = false;

void move_pen(int x_from, int y_from, int x_to, int y_to);
void move_piece(int x_from, int y_from, int x_to, int y_to);
void makeCommandOfMovePiece(int x_from, int y_from, int x_to, int y_to);

void print_board();

void updateObstaclesForMoving(int from_x, int from_y, int to_x, int to_y);
int findShortestPathWithLeastObstacles(int from_x, int from_y, int to_x, int to_y);

void onePlayerMove();
void moveForCommand();


void onePlayerMove()
{
    if(externalCommand.length() == 0) return;
    /*
    r m 1 2 3 4
    g t 1 2 3 4
    */
    int cur = 2;
    int x_from, y_from, x_to, y_to;

    cur = externalCommand.indexOf(' ', cur) + 1;
    x_from = externalCommand.charAt(cur) - '0';
    x_from = 9-x_from;

    cur = externalCommand.indexOf(' ', cur) + 1;
    y_from = externalCommand.charAt(cur) - '0';
    y_from = 8-y_from;

    cur = externalCommand.indexOf(' ', cur) + 1;
    x_to = externalCommand.charAt(cur) - '0';
    x_to = 9-x_to;

    cur = externalCommand.indexOf(' ', cur) + 1;
    y_to = externalCommand.charAt(cur) - '0';
    y_to = 8-y_to;

    // User's move
    if(externalCommand.charAt(0) == color)
    {
        board[x_from][y_from] = false;
        board[x_to][y_to] = true;

        updateObstaclesForMoving(x_from, y_from, x_to, y_to);

        command = "-END";
        return;
    }

    // Opposite user's move
    if(externalCommand.charAt(2) == 'm')
    {
        // move
        move_piece(x_from, y_from, x_to, y_to);
        delay(2000);
    }
    if(externalCommand.charAt(2) == 't')
    {
        // take
        move_piece(x_from, y_from, x_to, y_to);
        // take: push the target by 'move_piece'
        delay(5000); // The player should take the captured piece out
    }

    Serial.println("Done");
}



void move_pen(int x_from, int y_from, int x_to, int y_to)
{
    // Only when a piece is not attached
    if(x_from == x_to && y_from == y_to) return;

    int dx = (x_to - x_from) * FULL_SPACE_X;
    int dy = (y_to - y_from) * FULL_SPACE_Y;
    command += "|X" + String(dx) + " Y" + String(dy);
    // Serial.print("pen command: ");
    // Serial.println(command);
    moveForCommand();
    command = "X0";

    current_x = x_to;
    current_y = y_to;
}

void move_piece(int x_from, int y_from, int x_to, int y_to)
{
    if(x_from == x_to && y_from == y_to) return;
    move_pen(current_x, current_y, x_from, y_from);

    servoMove(PEN_UP);
    // the piece is attached to the pen now.

    if(findShortestPathWithLeastObstacles(x_from<<1, y_from<<1, x_to<<1, y_to<<1) != -1)
    {
        // diff x or y
        // same x or y but clear
        makeCommandOfMovePiece(x_from<<1, y_from<<1, x_to<<1, y_to<<1);
    }
    
    board[x_from][y_from] = false;
    board[x_to][y_to] = true;

    updateObstaclesForMoving(x_from, y_from, x_to, y_to);

    // Serial.print("piece command: ");
    // Serial.println(command);

    moveForCommand();
    servoMove(PEN_DOWN);

    current_x = x_to;
    current_y = y_to;
}

void makeCommandOfMovePiece(int x_from, int y_from, int x_to, int y_to)
{
    if(x_from == x_to && y_from == y_to) return;

    switch (prev_dir[x_to][y_to])
    {
        case UP:
            makeCommandOfMovePiece(x_from, y_from, x_to-1, y_to);
            command += "|X" + String(HALF_SPACE_X);
            break;
        case DOWN:
            makeCommandOfMovePiece(x_from, y_from, x_to+1, y_to);
            command += "|X" + String(-HALF_SPACE_X);
            break;
        case RIGHT:
            makeCommandOfMovePiece(x_from, y_from, x_to, y_to+1);
            command += "|Y" + String(-HALF_SPACE_Y);
            break;
        case LEFT:
            makeCommandOfMovePiece(x_from, y_from, x_to, y_to-1);
            command += "|Y" + String(HALF_SPACE_Y);
            break;
        default:
            break;
    }
}

void moveForCommand()
{
    String path = preParse();
    parsePath(path);
    startWork();
}

void print_board()
{
    for(int i=0;i<10;i++)
    {
        for(int j=0;j<9;j++) Serial.print(board[i][j]);
        Serial.println();
    }
}



void updateObstaclesForMoving(int x_from, int y_from, int x_to, int y_to)
{
    int xxf = x_from<<1;
    int yyf = y_from<<1;
    obstacles[xxf][yyf] = 0;
    if(xxf-1 >= 0 && obstacles[xxf-1][yyf] > 0) obstacles[xxf-1][yyf]--;
    if(xxf+1 < LEN_X && obstacles[xxf+1][yyf] > 0) obstacles[xxf+1][yyf]--;
    if(yyf-1 >= 0 && obstacles[xxf][yyf-1] > 0) obstacles[xxf][yyf-1]--;
    if(yyf+1 < LEN_Y && obstacles[xxf][yyf+1] > 0) obstacles[xxf][yyf+1]--;

    int xxt = x_to<<1;
    int yyt = y_to<<1;
    obstacles[xxt][yyt] = OCCUPIED;
    if(xxt-1 >= 0) obstacles[xxt-1][yyt]++;
    if(xxt+1 < LEN_X) obstacles[xxt+1][yyt]++;
    if(yyt-1 >= 0) obstacles[xxt][yyt-1]++;
    if(yyt+1 < LEN_Y) obstacles[xxt][yyt+1]++;
}

int findShortestPathWithLeastObstacles(int from_x, int from_y, int to_x, int to_y)
{
    int tmp = obstacles[to_x][to_y];
    obstacles[to_x][to_y] = 0;

    int dx = (from_x < to_x) ? 1 : -1;
    int dy = (from_y < to_y) ? 1 : -1;
    
    int prev_dir_x = (from_x < to_x) ? UP : DOWN;
    int prev_dir_y = (from_y < to_y) ? LEFT : RIGHT;

    dp[from_x][from_y] = obstacles[from_x][from_y];

    if(from_x == to_x)
    {
        bool clean = true;
        for(int j = from_y+dy; dy*j <= dy*to_y; j+=dy) if(obstacles[from_x][j] == OCCUPIED) { clean = false; break; }
        if(clean)
        {
            for(int j = from_y; dy*j <= dy*to_y; j+=dy) prev_dir[from_x][j] = prev_dir_y;
            return 0;
        }

        int upward_path = (from_x-1 >= 0) ? findShortestPathWithLeastObstacles(from_x-1, from_y, to_x, to_y) : MAX;
        int downward_path = (from_x+1 < LEN_X) ? findShortestPathWithLeastObstacles(from_x+1, from_y, to_x, to_y) : MAX;

        if(upward_path < downward_path)
        {
            command = "X" + String(-HALF_SPACE_X);
            findShortestPathWithLeastObstacles(from_x-1, from_y, to_x, to_y);
            makeCommandOfMovePiece(from_x-1, from_y, to_x, to_y);
            return -1;
        }
        else
        {
            command = "X" + String(HALF_SPACE_X);
            findShortestPathWithLeastObstacles(from_x+1, from_y, to_x, to_y);
            makeCommandOfMovePiece(from_x+1, from_y, to_x, to_y);
            return -1;
        }
    }
    if(from_y == to_y)
    {
        bool clean = true;
        for(int i = from_x+dx; dx*i <= dx*to_x; i+=dx) if(obstacles[i][from_y] == OCCUPIED) { clean = false; break; }
        if(clean)
        {
            for(int i = from_x; dx*i <= dx*to_x; i+=dx) prev_dir[i][from_y] = prev_dir_x;
            return 0;
        }

        int left_path = (from_y-1 >= 0) ? findShortestPathWithLeastObstacles(from_x, from_y-1, to_x, to_y) : MAX;
        int right_path = (from_y+1 < LEN_Y) ? findShortestPathWithLeastObstacles(from_x, from_y+1, to_x, to_y) : MAX;

        if(left_path < right_path)
        {
            command = "Y" + String(-HALF_SPACE_Y);
            findShortestPathWithLeastObstacles(from_x, from_y-1, to_x, to_y);
            makeCommandOfMovePiece(from_x, from_y-1, to_x, to_y);
            return -1;
        }
        else
        {
            command = "Y" + String(HALF_SPACE_Y);
            findShortestPathWithLeastObstacles(from_x, from_y+1, to_x, to_y);
            makeCommandOfMovePiece(from_x, from_y+1, to_x, to_y);
            return -1;
        }
    }

    // dynamic programming
    for(int i = from_x; dx*i <= dx*to_x; i+=dx)
    {
        for(int j = from_y; dy*j <= dy*to_y; j+=dy)
        {
            if(i == from_x && j == from_y) continue;

            int a = MAX;
            int b = MAX;

            if(i != from_x) a = dp[i-dx][j];
            if(j != from_y) b = dp[i][j-dy];

            dp[i][j] = min(a, b) + obstacles[i][j];
            prev_dir[i][j] = (a < b) ? prev_dir_x : prev_dir_y;
        }
    }

    obstacles[to_x][to_y] = tmp;
    return dp[to_x][to_y];
}

