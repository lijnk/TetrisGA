#ifndef TETROMINO_H
#define TETROMINO_H
#include <string.h>
#include <stdlib.h>

enum { TETRO_BL, TETRO_I, TETRO_O, TETRO_T, TETRO_L, TETRO_J, TETRO_Z, TETRO_S, TETRO_DB, TETRO_VD };

struct T;
typedef struct T Tetromino;

Tetromino* tetrominoInit(); //creates new tetromino
Tetromino* tetrominoCopy(Tetromino* ta); //copies tetromino from a to b
void tetrominoRotate(Tetromino* tetromino); //rotates tetromino
void tetrominoCycleType(Tetromino* tetromino); //cycles through different types (debug)
void tetrominoMoveX(Tetromino* tetromino, int x); //moves tetromino horizontally
void tetrominoSetX(Tetromino* tetromino, int x); //manually sets tetromino's x coord
void tetrominoMoveY(Tetromino* tetromino, int y); //moves tetromino vertically
void tetrominoSetY(Tetromino* tetromino, int y); //manually set tetromino's y coord
void tetrominoSetType(Tetromino* tetromino, int type); //set tetromino's type
void tetrominoSetAngle(Tetromino* tetromino, int angle); //set tetromino's angle

int tetrominoGetX(Tetromino* tetromino); //get x
int tetrominoGetY(Tetromino* tetromino); //get y
unsigned int tetrominoGetType(Tetromino* tetromino); //get type
unsigned int tetrominoGetAngle(Tetromino* tetromino); //get angle
int tetrominoGetMino(Tetromino* tetromino, int x, int y); //return if a mino is in [x,y] of tetromino square

size_t tetrominoSizeof(); //sizeof

#endif

