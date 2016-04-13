#include "tetromino.h"

struct T {
	unsigned int type;
	int x;
	int y;
	unsigned int angle;
};

//shapes
static const char* MINOS[] = {
	"", //filler
	"    ****        ", //I
	"     **  **     ", //O
	"   *** * ", //T
	"   ****  ", //L
	"   ***  *", //J
	"    **** ", //Z
	"   **  **" //S
};

//rotational matrices
static const int RMAT_M[][9] = {
	{ 0, 1, 2, 3, 4, 5, 6, 7, 8 },
	{ 6, 3, 0, 7, 4, 1, 8, 5, 2 },
	{ 8, 7, 6, 5, 4, 3, 2, 1, 0 },
	{ 2, 5, 8, 1, 4, 7, 0, 3, 6 }
};

//rotational matrices for I and O blocks
static const int RMAT_IO[][16] = {
	{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
	{ 12, 8, 4, 0, 13, 9, 5, 1, 14, 10, 6, 2, 15, 11, 7, 3 }
};

Tetromino* tetrominoInit()
{
	//initialise tetromino
	Tetromino* r = (Tetromino*)malloc(sizeof(Tetromino));
	r->type = 1;
	r->angle = 0;
	r->x = 0;
	r->y = 0;

	return r;
}

Tetromino* tetrominoCopy(Tetromino* ta)
{
	//copy tetromino pointer
	Tetromino* tb = (Tetromino*)malloc(sizeof(Tetromino));
	tb->type = ta->type;
	tb->angle = ta->angle;
	tb->x = ta->x;
	tb->y = ta->y;

	return tb;
}

void tetrominoRotate(Tetromino* tetromino)
{
	tetromino->angle = (tetromino->angle + 1) % 4; //4 angles
}

void tetrominoCycleType(Tetromino* tetromino)
{
	tetromino->type = (tetromino->type + 1) % 7; //7 types
}

void tetrominoMoveX(Tetromino* tetromino, int x)
{
	tetromino->x += x; //move relative to x position
}

void tetrominoSetX(Tetromino* tetromino, int x)
{
	tetromino->x = x; //set new x position
}

void tetrominoMoveY(Tetromino* tetromino, int y)
{
	tetromino->y += y; //move relative to y position
}

void tetrominoSetY(Tetromino* tetromino, int y)
{
	tetromino->y = y; //set new y position
}

void tetrominoSetType(Tetromino* tetromino, int type)
{
	tetromino->type = type; //set new type
}

void tetrominoSetAngle(Tetromino* tetromino, int angle)
{
	tetromino->angle = angle % 4; //set new angle
}

int tetrominoGetX(Tetromino* tetromino)
{
	return tetromino->x;
}

int tetrominoGetY(Tetromino* tetromino)
{
	return tetromino->y;
}

unsigned int tetrominoGetType(Tetromino* tetromino)
{
	return tetromino->type;
}

unsigned int tetrominoGetAngle(Tetromino* tetromino)
{
	return tetromino->angle;
}

//get tetromino from the 4x4 grid
int tetrominoGetMino(Tetromino* tetromino, int x, int y)
{
	if(x < 0 || x > 3 || y < 0 || y > 3) return 0;

	if(tetromino->type == TETRO_O) //O has one size
	{
		return MINOS[TETRO_O][RMAT_IO[0][(y * 4) + x]] != ' ';
	}
	else if(tetromino->type == TETRO_I) //I has another size
	{
		return MINOS[TETRO_I][RMAT_IO[tetromino->angle % 2][(y * 4) + x]] != ' ';
	}
	else //the rest have their own size
	{
		if(x == 3 || y == 3) return 0;
		return MINOS[tetromino->type][RMAT_M[tetromino->angle][(y * 3) + x]] != ' ';
	}
}

size_t tetrominoSizeof()
{
	return sizeof(Tetromino);
}
