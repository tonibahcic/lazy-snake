#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <curses.h>

#define WIDTH 20
#define HEIGHT 20
#define SNAKE_MAX_LENGHT 100
#define COO_9999 9999

#define DEFAULT_GRID ' '
#define SNAKE_HEAD 'O'
#define SNAKE_TAIL 'o'
#define CANDY '*'

struct coordinate {
    int x;
    int y;
} SNAKE_ARRAY[SNAKE_MAX_LENGHT];

int HEAD_X = 9;
int HEAD_Y = 10;

char COMMAND = 'x';

int CANDY_X = 5;
int CANDY_Y = 15;
int SCORE = 0;

int GAME_IN_PROGRESS = 1;


void initSnake();
void drawHeader();
void drawMatrix();
int isHeadHere(int x, int y);
int isSnakeHere(int x, int y);
int isCandyHere(int x, int y);
void readInput();
int willHitNeck(int x, int y);
int willHitTail(int x, int y);
int moveHead();
void moveTail();
void checkCandy();

int main() {
    initscr();
    cbreak();
    noecho();

    initSnake();

    while (GAME_IN_PROGRESS)
    {
        drawHeader();
        drawMatrix();
        readInput();
        checkCandy();
    }

    printw("Game over! Score: %d", SCORE);

    getch();
    endwin();
    return 0;
}

void initSnake() {
    for (int i = 0; i <= SNAKE_MAX_LENGHT; i++) {
        SNAKE_ARRAY[i].x = COO_9999;
        SNAKE_ARRAY[i].y = COO_9999;
    }
    SNAKE_ARRAY[0].x = HEAD_X;
    SNAKE_ARRAY[0].y = HEAD_Y;
}

void drawHeader() {
    printw("\n");
    printw(" Last key: %c", COMMAND);
    printw("\t\t\t\t\t Score: %.3d", SCORE);
    printw("\n");
}

void drawMatrix() {
    for (int i = HEIGHT-1; i >= 0; i--) {
        for (int j = 0; j < WIDTH; j++) {
            char toPrint = DEFAULT_GRID;

            if (isHeadHere(j, i)) {
                toPrint = SNAKE_HEAD;
            } else if (isSnakeHere(j, i)) {
                toPrint = SNAKE_TAIL; 
            } else if (isCandyHere(j, i)) {
                toPrint = CANDY;
            }

            printw(" %c ", toPrint);
        }
        printw("\n");
    }
}

int isHeadHere(int x, int y) {
    return x == HEAD_X && y == HEAD_Y;
}

int isSnakeHere(int x, int y) {
    int is_hit = 0;

    for (int i = 0; i <= SNAKE_MAX_LENGHT; i++) {
        if (x == SNAKE_ARRAY[i].x && y == SNAKE_ARRAY[i].y) {
            is_hit = 1;
        }
    }

    return is_hit;
}

int isCandyHere(int x, int y) {
    return x == CANDY_X && y == CANDY_Y;
}

void readInput() {
    // A = Up
    // B = Down   
    // C = Right
    // D = Left

    COMMAND = getch();

    // Ignore other characters
    if (
        COMMAND != 'A' &&
        COMMAND != 'B' && 
        COMMAND != 'C' && 
        COMMAND != 'D'
    ) {
        return;
    }

    int success = moveHead(COMMAND);
    if (success) moveTail();

    clear();
}

int moveHead() {
    int wantedX = HEAD_X;
    int wantedY = HEAD_Y;
    
    // Adjust head
    if (COMMAND == 'A') wantedY++;
    if (COMMAND == 'B') wantedY--;
    if (COMMAND == 'C') wantedX++;
    if (COMMAND == 'D') wantedX--;

    if (willHitNeck(wantedX, wantedY)) {
        clear();
        return 0;
    } else if (willHitTail(wantedX, wantedY)) {
        clear();
        GAME_IN_PROGRESS = 0;
        return 0;
    } else {
        HEAD_X = wantedX;
        HEAD_Y = wantedY;
    }


    // Portal mode
    if (HEAD_X == WIDTH) HEAD_X = 0;
    if (HEAD_X < 0) HEAD_X = WIDTH - 1;
    if (HEAD_Y == HEIGHT) HEAD_Y = 0;
    if (HEAD_Y < 0) HEAD_Y = HEIGHT - 1;

    return 1;
}

int willHitNeck(int x, int y) {
    int neckX = SNAKE_ARRAY[1].x;
    int neckY = SNAKE_ARRAY[1].y;

    if (x == neckX && y == neckY) {
        return 1;
    } else {
        return 0;
    }
}

int willHitTail(int x, int y) {
    // Ignore head and neck
    for (int i = 2; i <= SNAKE_MAX_LENGHT; i++) {
        if (SNAKE_ARRAY[i].x == x && SNAKE_ARRAY[i].y == y) {
            return 1;
        }
    }

    return 0;
}

void moveTail() {
    // Shift snake right
    for (int i = SNAKE_MAX_LENGHT; i > 0; i--) {
        SNAKE_ARRAY[i].x = SNAKE_ARRAY[i-1].x;
        SNAKE_ARRAY[i].y = SNAKE_ARRAY[i-1].y;
    }

    // Add current head to start
    SNAKE_ARRAY[0].x = HEAD_X;
    SNAKE_ARRAY[0].y = HEAD_Y;

    // Remove tail if we didnt hit candy
    if (HEAD_X != CANDY_X || HEAD_Y != CANDY_Y) {
        for (int i = SNAKE_MAX_LENGHT; i > 0; i--) {
            if (SNAKE_ARRAY[i].x != COO_9999 || SNAKE_ARRAY[i].y != COO_9999) {
                SNAKE_ARRAY[i].x = COO_9999;
                SNAKE_ARRAY[i].y = COO_9999;
                break;
            }
        }
    }
}

void checkCandy() {
    if (isCandyHere(HEAD_X, HEAD_Y)) {
        SCORE++;

        int isSpawnedOnSnake = 0;

        do {
            // Set candy to a new place
            int MODULO_X = HEIGHT - 2;
            CANDY_X = (rand() % MODULO_X) + 1;
            int MODULO_Y = WIDTH - 2;
            CANDY_Y = (rand() % MODULO_Y) + 1;

            // Check if candy is spawned on snake
            for (int i = 0; i <= SNAKE_MAX_LENGHT; i++) {
                if (SNAKE_ARRAY[i].x == CANDY_X && SNAKE_ARRAY[i].y == CANDY_Y) {
                    isSpawnedOnSnake = 1;
                    break;
                }
            }
        } while(isSpawnedOnSnake);
    }
}
