#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "tetromino.h"
#include "text.h"
#include "ui.h"
#include "console.h"
#include "gvars.h"
#include "events.h"

static const Pix BUTTONS[] = {
	{0.3f, 0.3f, 0.3f, 1.f}, //regular button
	{0.5f, 0.3f, 0.3f, 1.f}, //red button
	{0.3f, 0.5f, 0.3f, 1.f}, //green button
	{0.3f, 0.3f, 0.3f, 1.f}, //banner colour
	{0.1f, 0.1f, 0.1f, 1.f} //selector colour
};

//button colours corresponding to the above
enum {
	BUTTON_REG, 
	BUTTON_RED,
	BUTTON_GREEN,
	BANNER,
	SELECTOR
};

void graphicsInit(); //initialises the font and ui
void drawSetReq(); //request for drawing (helps not draw all the time)
void drawResetReq();
int drawGetReq();

void drawBorder(Pix* screen, int x, int y, int width, int height); //draw border
void drawMino(Pix* screen, int x, int y, int type); //draws individual mino block
void drawTetromino(Pix* screen, Tetromino* tetromino, int xoff, int yoff, int bg); //draw full tetromino block
void drawText(int x, int y, const char* text, Pix colour); //draws text on screen
void drawLabel(int tb); //draws label on screen (centre justified)
void drawButton(int tb); //draws button on screen (centre justified)
void drawSelector(int tb); //draws selector (centre justified)
void drawTextbox(int tb); //draws text box (left justified)
void drawScrollbar(int sb); //draws scroll bar
void drawBG(Pix bg); //draws a complete background in given colour
void drawClearScreen(Pix* screen); //clears screen
void drawSaveLoadScreen(int saveload); //draws save/load screen. 1 for save, 0 for load
void drawUI(); //draws UI portion (also includes console)
void drawConsole(); //draws console overlay

#endif

