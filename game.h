#ifndef GAME_H
#define GAME_H

#define DEBUG 1

#include <GLFW/glfw3.h>
#include <stdio.h>
#include <time.h>
#include "tetromino.h"
#include "console.h"
#include "gvars.h"
#include "graphics.h"
#include "events.h"

struct G;
typedef struct G Game;

enum {GAME_LEFT, GAME_RIGHT, GAME_ROTATE, GAME_DOWN, GAME_DROP, GAME_PAUSE, GAME_RESTART};

int gameInit(Game* game); //initialises game
void gamePause(Game* game); //explicitly pauses game
void gameUnpause(Game* game); //explicitly unpauses game
void gameRestart(Game* game); //restarts game
void gameTick(Game* game); //ticks game
void gameMove(Game* game, int dir); //moves piece left -, right +
int gameEvent(Game* game, int ev); //performs event
void gameDraw(Game* game); //draw commands
void gameClean(Game* game); //clean up game

int gameGetMinoX(Game* game); //gets active mino's x
int gameGetMinoY(Game* game); //gets active mino's y
int gameGetGameover(Game* game); //gets gameover state
int gameGetScore(Game* game); //returns score
int gameGetPause(Game* game); //returns if game is paused
int gameGetTile(Game* game, int x, int y); //returns tile at coords
int gameGetExtendedTile(Game* game, int x, int y); //returns extended tile at coords (active mino)
Pix* gameGetBG(Game* game); //get background
Pix* gameGetFG1(Game* game); //get foreground 1
Pix* gameGetFG2(Game* game); //get foreground 2
size_t gameSizeof(); //return size of game

#endif

