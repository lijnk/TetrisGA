#include "game.h"

//game
struct G {
	int** board;
	Pix* bg; //background
	Pix* fg1; //foreground 1
	Pix* fg2; //foreground 2
	Tetromino* activeMino; //the tetromino we can move
	Tetromino* nextMino; //the tetromino in the box
	int* history; //the last several tetrominos we got (used so we don't get like 50 doubles in a row)
	int* stats; //tick, score, level, lines, gameover
};

enum {GF_TICK, GF_SCORE, GF_LEVEL, GF_LINES, GF_GAMEOVER, GF_PAUSED};

//how many ticks we go before we drop
static const int GAME_SPEED[] = {53, 49, 45, 41, 27, 33, 28, 22, 17, 11, 10, 9, 8, 7, 6, 6, 5, 4, 4, 3};

void gameSetTile(Game* game, int x, int y, int value);
int collisionCheck(Game* game, Tetromino* tetromino);

int gameSetTetromino(Game* game)
{
	tetrominoSetType(game->activeMino, tetrominoGetType(game->nextMino));
	int newType = 0;

	//go through 32 attempts to grab a new tetromino (lessen chance of doubles)
	for(int i = 0; i < 32; i++)
	{
		newType = (rand() % 7) + 1;
		if (newType != game->history[0] ||
			newType != game->history[1] ||
			newType != game->history[2] ||
			newType != game->history[3])
		{
			break;
		}
	}

	game->history[3] = game->history[2]; //shift history
	game->history[2] = game->history[1];
	game->history[1] = game->history[0];
	game->history[0] = newType;

	tetrominoSetType(game->nextMino, newType);
	tetrominoSetX(game->activeMino, 3);
	tetrominoSetY(game->activeMino, 14);
	tetrominoSetAngle(game->activeMino, 0);

	//these probably shouldn't be here...
	if(GRAPHICS_HEADLESS) return 0;
	drawTetromino(game->fg1, game->nextMino, MINO_X+CELL_SIZE/2, MINO_Y+CELL_SIZE/2, 1);

	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 18; j++)
		{
			int value = gameGetTile(game, i, j);
			drawMino(game->bg, GB_X+(CELL_SIZE*i)+BORDER/2, GB_Y+(CELL_SIZE*j)+BORDER/2, value);
		}
	}
}

int gameInit(Game* game)
{
	//initialise game memory
	Pix c = {1.f, 1.f, 1.f, 1.f};
	srand(50); //makes this a bit more predictable (probably doesn't help though)
	game->board = (int**)calloc(18, sizeof(int*));
	for(int i = 0; i < 18; i++)
	{
		game->board[i] = (int*)calloc(10, sizeof(int));
	}

	game->bg = (Pix*)calloc(WINDOW_WIDTH * WINDOW_HEIGHT, sizeof(Pix));
	game->fg1 = (Pix*)calloc(WINDOW_WIDTH * WINDOW_HEIGHT, sizeof(Pix));
	game->fg2 = (Pix*)calloc(WINDOW_WIDTH * WINDOW_HEIGHT, sizeof(Pix));
	game->history = (int*)calloc(4, sizeof(int));
	game->stats = (int*)calloc(6, sizeof(int)); //tick, score, levels, lines, gameover, paused

	//set active tetromino
	game->activeMino = tetrominoInit();
	tetrominoSetX(game->activeMino, 3);
	tetrominoSetY(game->activeMino, 14);

	//set next tetromino
	game->nextMino = tetrominoInit();
	tetrominoSetType(game->nextMino, (rand() % 7) + 1);
	game->history[0] = tetrominoGetType(game->nextMino);

	//set types for both next and active
	gameSetTetromino(game);

	if(GRAPHICS_HEADLESS) return 0;
	drawBorder(game->bg, GB_X, GB_Y, GB_WIDTH, GB_HEIGHT); //game border
	drawBorder(game->bg, STATS_X, STATS_Y, STATS_WIDTH, STATS_HEIGHT); //score
	drawBorder(game->bg, MINO_X, MINO_Y, MINO_WIDTH, MINO_HEIGHT); //next mino
	drawBorder(game->bg, NET_X, NET_Y, NET_WIDTH, NET_HEIGHT); //NEAT network
	//drawText(NET_X+CELL_SIZE*2, NET_Y+NET_HEIGHT+BORDER, "NETWORK", c); //Network text
}

void gamePause(Game* game)
{
	game->stats[GF_PAUSED] = 1;
}

void gameUnpause(Game* game)
{
	game->stats[GF_PAUSED] = 0;
}

void gameRestart(Game* game)
{
	gameClean(game);
	gameInit(game);
}

int checkLine(Game* game, int y)
{
	//check if we made a line at y
	for(int i = 0; i < 10; i++)
	{
		if(gameGetTile(game, i, y) == 0) return 0;
	}

	return 1;
}

void deleteLine(Game* game, int y)
{
	free(game->board[y]); //delete line

	//shift rows down
	for(int i = y; i < 17; i++)
	{
		game->board[i] = game->board[i+1];
	}

	//create new top row
	game->board[17] = (int*)calloc(10, sizeof(int));
}

int clearLines(Game* game)
{
	//could probably be optimised by checking where we landed
	int result = 0;
	for(int i = 0; i < 18; i++)
	{
		if(checkLine(game, i))
		{
			result++;
			deleteLine(game, i--);
		}
	}

	return result;
}

void gameTick(Game* game)
{
	//don't tick for pause or gameover
	if(game->stats[GF_PAUSED]) return;
	if(game->stats[GF_GAMEOVER]) return;

	//check if we move down
	if(game->stats[GF_TICK] % GAME_SPEED[game->stats[GF_LEVEL]] == 0)
	{
		Tetromino* temp = tetrominoCopy(game->activeMino);

		tetrominoMoveY(temp, -1);
		if(collisionCheck(game, temp)) //check if we've landed
		{
			for(int i = 0; i < 4; i++)
			{
				for(int j = 0; j < 4; j++)
				{
					if(tetrominoGetMino(game->activeMino, i, j))
					{
						int tx = tetrominoGetX(game->activeMino);
						int ty = tetrominoGetY(game->activeMino);
						gameSetTile(game, i+tx, j+ty, tetrominoGetType(game->activeMino)); //set tetromino into board
					}
				}
			}
			//clear lines if we have any and adjust score
			int lines = clearLines(game);
			switch(lines)
			{
				case 1:
					game->stats[GF_SCORE] += 40 * (game->stats[GF_LEVEL] + 1);
					break;
				case 2:
					game->stats[GF_SCORE] += 100 * (game->stats[GF_LEVEL] + 1);
					break;
				case 3:
					game->stats[GF_SCORE] += 300 + (game->stats[GF_LEVEL] + 1);
					break;
				case 4:
					game->stats[GF_SCORE] += 1200 + (game->stats[GF_LEVEL] + 1);
					break;
			}

			//line/level recalc
			game->stats[GF_LINES] += lines;
			if((game->stats[GF_LINES] / 10 > game->stats[GF_LEVEL]) && game->stats[GF_LEVEL] < 20)
			{
				game->stats[GF_LEVEL] = game->stats[GF_LINES] / 10;
			}

			//set tetromino into board, get new tetromino, gameover check
			gameSetTetromino(game);
			if(collisionCheck(game, game->activeMino))
			{
				game->stats[GF_GAMEOVER] = 1;
			}
			//printf("Score: %d\tLines: %d\tLevel: %d\n", game->stats[GF_SCORE], game->stats[GF_LINES], game->stats[GF_LEVEL]);
		}
		else
		{
			tetrominoMoveY(game->activeMino, -1);
		}
	}

	game->stats[GF_TICK]++;
}

int collisionCheck(Game* game, Tetromino* tetromino)
{
	//check if the tetromino is colliding with the game
	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			if(tetrominoGetMino(tetromino, i, j))
			{
				int x = tetrominoGetX(tetromino);
				int y = tetrominoGetY(tetromino);

				if((i + x) < 0) return 1; //left wall
				if((i + x) > 9) return 2; //right wall
				if((j + y) < 0) return 3; //floor
				if(gameGetTile(game, x + i, y + j) != TETRO_BL) return 4; //tiles
			}
		}
	}
	return 0;
}

void gameMove(Game* game, int dir)
{
	//move logic to avoid collisions
	Tetromino* temp = tetrominoCopy(game->activeMino);
	if(dir >= 0) //right
	{
		tetrominoMoveX(temp, 1);
		if(!collisionCheck(game, temp))
			tetrominoMoveX(game->activeMino, 1);
	}
	else //left
	{
		tetrominoMoveX(temp, -1);
		if(!collisionCheck(game, temp))
			tetrominoMoveX(game->activeMino, -1);
	}

	free(temp);
}

void gameDrop(Game* game)
{
	//drop the tetromino to the bottom and make sure we have a little bit of time to mess with it
	Tetromino* temp = tetrominoCopy(game->activeMino);

	while(!collisionCheck(game, temp))
	{
		tetrominoMoveY(temp, -1);
	}

	if(tetrominoGetY(game->activeMino) == tetrominoGetY(temp) + 1) return; //drop glitch fix so spamming the drop key doesn't mean free time

	tetrominoSetY(game->activeMino, tetrominoGetY(temp) + 1);
	//adjust tick
	game->stats[GF_TICK] += GAME_SPEED[game->stats[GF_LEVEL]] - (game->stats[GF_TICK] % GAME_SPEED[game->stats[GF_LEVEL]]) + 1;
}

void gameRotate(Game* game)
{
	//rotate logic to avoid collisions
	Tetromino* temp = tetrominoCopy(game->activeMino);
	tetrominoRotate(temp);
	switch(collisionCheck(game, temp))
	{
		case 0: //nothing. we're free
			tetrominoRotate(game->activeMino);
			break;
		case 1: //left wall
			tetrominoMoveX(temp, 1);
			if(collisionCheck(game, temp))
			{
				tetrominoMoveX(temp, 1); //might be an I
				if(collisionCheck(game, temp)) break;
				tetrominoMoveX(game->activeMino, 2);
				tetrominoRotate(game->activeMino);
				break;
			}
			tetrominoMoveX(game->activeMino, 1);
			tetrominoRotate(game->activeMino);
			break;
		case 2: //right wall
			tetrominoMoveX(temp, -1);
			if(collisionCheck(game, temp))
			{
				tetrominoMoveX(temp, -1); //might be an I
				if(collisionCheck(game, temp)) break;
				tetrominoMoveX(game->activeMino, -2);
				tetrominoRotate(game->activeMino);
				break;
			}
			tetrominoMoveX(game->activeMino, -1);
			tetrominoRotate(game->activeMino);
			break;
		//ignore tile collisions (might not be the best for walls of blocks, but game works well enough)
	}

	free(temp);
}

int gameEvent(Game* game, int ev)
{
	//event handling
	if(game->stats[GF_PAUSED]) //pause can only unpause or restart
	{
		if(ev == GAME_RESTART) gameRestart(game);
		else if(ev == GAME_PAUSE) gameUnpause(game);
		return 0;
	}
	if(game->stats[GF_GAMEOVER]) //gameover can only restart
	{
		if(ev == GAME_RESTART)
			gameRestart(game);
		return 0;
	}

	switch(ev)
	{
		case GAME_LEFT: //left
			gameMove(game, -1);
			break;
		case GAME_RIGHT: //right
			gameMove(game, 1);
			break;
		case GAME_ROTATE: //up
			gameRotate(game);
			break;
		case GAME_DOWN: //down
			game->stats[GF_TICK] += GAME_SPEED[game->stats[GF_LEVEL]] - (game->stats[GF_TICK] % GAME_SPEED[game->stats[GF_LEVEL]]);
			break;
		case GAME_DROP: //space
			gameDrop(game);
			break;
		case GAME_PAUSE: //some onscreen button or p maybe
			gamePause(game);
			break;
		case GAME_RESTART: //r
			gameRestart(game);
			break;
	}
	return 0;
}

void gameDraw(Game* game)
{
	//draw board
	Pix c = {1.f, 1.f, 1.f, 1.f};

	//clear sprites on board
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 18; j++)
		{
			drawMino(game->fg1, GB_X+(CELL_SIZE*i)+BORDER/2, GB_Y+(CELL_SIZE*j)+BORDER/2, TETRO_VD);
		}
	}

	//text
	char stat[14];
	sprintf(stat, "Score: %d", game->stats[GF_SCORE]);
	drawText(STATS_X + CELL_SIZE, STATS_Y + TEXT_HEIGHT*3, stat, c);
	sprintf(stat, "Lines: %d", game->stats[GF_LINES]);
	drawText(STATS_X + CELL_SIZE, STATS_Y + TEXT_HEIGHT*2, stat, c);
	sprintf(stat, "Level: %d", game->stats[GF_LEVEL]);
	drawText(STATS_X + CELL_SIZE, STATS_Y + TEXT_HEIGHT*1, stat, c);

	//active tetromino
	drawTetromino(game->fg1, game->activeMino, GB_X+BORDER/2, GB_Y+BORDER/2, 1);
}

void gameClean(Game* game)
{
	//clean up
	free(game->board);
	free(game->activeMino);
}

//get stuff
int gameGetMinoX(Game* game)
{
	return tetrominoGetX(game->activeMino);
}

int gameGetMinoY(Game* game)
{
	return tetrominoGetY(game->activeMino);
}

int gameGetGameover(Game* game)
{
	return game->stats[GF_GAMEOVER];
}

int gameGetScore(Game* game)
{
	return game->stats[GF_SCORE];
}

int gameGetPause(Game* game)
{
	return game->stats[GF_PAUSED];
}

int gameGetTile(Game* game, int x, int y)
{
	//return tile on board
	if(x < 0 || x > 9 || y < 0 || y > 17) return 0;
	return game->board[y][x];
}

//set stuff
void gameSetTile(Game* game, int x, int y, int value)
{
	//sets tile on board
	if (x < 0 || x > 9 || y < 0 || y > 17) return;
	game->board[y][x] = value;
}

int gameGetExtendedTile(Game* game, int x, int y)
{
	//returns extended tile info
	int mx = x - tetrominoGetX(game->activeMino);
	int my = y - tetrominoGetY(game->activeMino);

	return tetrominoGetMino(game->activeMino, mx, my);
}

//get graphics stuff
Pix* gameGetBG(Game* game)
{
	return game->bg;
}

Pix* gameGetFG1(Game* game)
{
	return game->fg1;
}

Pix* gameGetFG2(Game* game)
{
	return game->fg2;
}

//for a malloc somewhere
size_t gameSizeof(Game* game)
{
	return sizeof(Game);
}
