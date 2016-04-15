#include "graphics.h"

static const Pix COLOURS[] = {
	{ 0.2f, 0.2f, 0.2f, 1.0f }, //border
	{ 0.25f, 0.25f, 0.25f, 1.0f }, //lower
	{ 0.3f, 0.3f, 0.3f, 1.0f }, //upper

	{ 0.0f, 1.0f, 1.0f, 1.0f }, //cyan
	{ 0.5f, 1.0f, 1.0f, 1.0f }, //light
	{ 0.0f, 0.5f, 0.5f, 1.0f }, //dark

	{ 1.0f, 1.0f, 0.0f, 1.0f }, //yellow
	{ 1.0f, 1.0f, 0.5f, 1.0f }, //light
	{ 0.5f, 0.5f, 0.0f, 1.0f }, //dark

	{ 1.0f, 0.0f, 1.0f, 1.0f }, //purple
	{ 1.0f, 0.5f, 1.0f, 1.0f }, //light
	{ 0.5f, 0.0f, 0.5f, 1.0f }, //dark

	{ 1.0f, 0.5f, 0.0f, 1.0f }, //orange
	{ 1.0f, 0.75f, 0.5f, 1.0f }, //light
	{ 0.5f, 0.25f, 0.0f, 1.0f }, //dark

	{ 0.0f, 0.0f, 1.0f, 1.0f }, //blue
	{ 0.5f, 0.5f, 1.0f, 1.0f }, //light
	{ 0.0f, 0.0f, 0.5f, 1.0f }, //dark

	{ 1.0f, 0.0f, 0.0f, 1.0f }, //red
	{ 1.0f, 0.5f, 0.5f, 1.0f }, //light
	{ 0.5f, 0.0f, 0.0f, 1.0f }, //dark

	{ 0.0f, 1.0f, 0.0f, 1.0f }, //green
	{ 0.5f, 1.0f, 0.5f, 1.0f }, //light
	{ 0.0f, 0.5f, 0.0f, 1.0f }, //dark

	{ 0.9f, 0.9f, 0.9f, 1.0f },    //white (debug)
	{ 0.95f, 0.95f, 0.95f, 1.0f }, //light
	{ 0.85f, 0.85f, 0.85f, 1.0f },  //dark

	{ 0.0f, 0.0f, 0.0f, 0.0f }, //black (void) 
	{ 0.0f, 0.0f, 0.0f, 0.0f }, //black
	{ 0.0f, 0.0f, 0.0f, 0.0f } //black
};

static int drawReq;

void graphicsInit()
{
	//initialise the font and ui
	initFont();
	uiInit();

	//set up draw request variable
	drawReq = 0;

	//Add the functions for buttons
	//main screen
	uiSetContext(UI_CONTEXT_MAIN);
	uiAddButton("New", NEW_BUTTON_X, NEW_BUTTON_Y, UI_BUTTON_W, UI_BUTTON_H, BUTTONS[BUTTON_REG], eventNewButton);
	uiAddButton("Save", SAVE_BUTTON_X, SAVE_BUTTON_Y, UI_BUTTON_W, UI_BUTTON_H, BUTTONS[BUTTON_REG], eventSaveButton);
	uiAddButton("Load", LOAD_BUTTON_X, LOAD_BUTTON_Y, UI_BUTTON_W, UI_BUTTON_H, BUTTONS[BUTTON_REG], eventLoadButton);
	uiAddButton("Pause", PAUSE_BUTTON_X, PAUSE_BUTTON_Y, UI_BUTTON_W, UI_BUTTON_H, BUTTONS[BUTTON_GREEN], eventPause);
	uiSetInactiveButton("Pause");
	uiAddButton("Unpause", PAUSE_BUTTON_X, PAUSE_BUTTON_Y, UI_BUTTON_W, UI_BUTTON_H, BUTTONS[BUTTON_RED], eventUnpause);
	uiAddButton("Best", BEST_BUTTON_X, BEST_BUTTON_Y, UI_BUTTON_W, UI_BUTTON_H, BUTTONS[BUTTON_REG], eventBest);
	uiAddButton("Quit", QUIT_BUTTON_X, QUIT_BUTTON_Y, UI_BUTTON_W, UI_BUTTON_H, BUTTONS[BUTTON_RED], eventQuit);

	//save screen
	uiSetContext(UI_CONTEXT_SAVE);
	uiAddLabel("Save File?", 0, SAVELOAD_BANNER_Y, WINDOW_WIDTH, UI_BUTTON_H, BUTTONS[BANNER]);
	uiAddButton("Save", SAVELOAD_BUTTON_X, SAVELOAD_BUTTON_Y, UI_BUTTON_W, UI_BUTTON_H, BUTTONS[BUTTON_GREEN], eventSaveSaveButton);
	uiAddButton("Cancel", CANCEL_BUTTON_X, CANCEL_BUTTON_Y, UI_BUTTON_W, UI_BUTTON_H, BUTTONS[BUTTON_RED], eventCancelButton);
	uiAddTextbox("savefn", SAVELOAD_DATA_X, SAVELOAD_DATA_Y, SAVELOAD_DATA_W, SAVELOAD_DATA_H, BUTTONS[BUTTON_REG], "save.dat");

	//load screen
	uiSetContext(UI_CONTEXT_LOAD);
	uiAddLabel("Load File?", 0, SAVELOAD_BANNER_Y, WINDOW_WIDTH, UI_BUTTON_H, BUTTONS[BANNER]);
	uiAddButton("Load", SAVELOAD_BUTTON_X, SAVELOAD_BUTTON_Y, UI_BUTTON_W, UI_BUTTON_H, BUTTONS[BUTTON_GREEN], eventLoadLoadButton);
	uiAddButton("Cancel", CANCEL_BUTTON_X, CANCEL_BUTTON_Y, UI_BUTTON_W, UI_BUTTON_H, BUTTONS[BUTTON_RED], eventCancelButton);
	//load screen file buttons
	for(int i = 0; i < 8; i++)
	{
		uiAddSelector("loadfn", SAVELOAD_DATA_X, SAVELOAD_DATA_Y - ((SAVELOAD_DATA_H + 4) * i), SAVELOAD_DATA_W, SAVELOAD_DATA_H, BUTTONS[SELECTOR]);
	}
	uiAddScrollbar("loadfn", LOADSCROLL_X, LOADSCROLL_Y, LOADSCROLL_W, LOADSCROLL_H, BUTTONS[BUTTON_REG]);

	//new screen
	uiSetContext(UI_CONTEXT_NEW);
	uiAddLabel("Save File?", 0, SAVELOAD_BANNER_Y, WINDOW_WIDTH, UI_BUTTON_H, BUTTONS[BANNER]);
	uiAddButton("Save", SAVELOAD_BUTTON_X, SAVELOAD_BUTTON_Y, UI_BUTTON_W, UI_BUTTON_H, BUTTONS[BUTTON_GREEN], eventNewSave);
	uiAddButton("Discard", CANCEL_BUTTON_X, CANCEL_BUTTON_Y, UI_BUTTON_W, UI_BUTTON_H, BUTTONS[BUTTON_RED], eventNewCancel);
	uiAddTextbox("savefn", SAVELOAD_DATA_X, SAVELOAD_DATA_Y, SAVELOAD_DATA_W, SAVELOAD_DATA_H, BUTTONS[BUTTON_REG], "save.dat");

	uiSetContext(UI_CONTEXT_MAIN);
}

void drawSetReq(int v)
{
	drawReq |= v;
}

void drawResetReq()
{
	drawReq = 0;
}

int drawGetReq()
{
	return drawReq;
}

//safe way to draw pixels on screen (does wraparound if we go over)
void drawPix(Pix* screen, int x, int y, Pix value)
{
	x = (x % WINDOW_WIDTH);
	y = (y % WINDOW_HEIGHT);

	if(x < 0) x += WINDOW_WIDTH;
	if(y < 0) y += WINDOW_HEIGHT;

	screen[(y*WINDOW_WIDTH) + x] = value;
}

void drawBorder(Pix* screen, int x, int y, int width, int height)
{
	Pix a, b, c, d;

	a.r = a.g = a.b = 1.f; //white
	b.r = b.g = b.b = 0.f; //black
	c.r = c.g = c.b = 0.2f; //dark grey
	d.r = d.g = d.b = 0.4f; //light grey
	a.a = b.a = c.a = d.a = 1.0f; //alpha

	int cx = x;
	int cy = y;

	//bottom
	//row 1
	drawPix(screen, cx++, cy, b);
	drawPix(screen, cx++, cy, b);

	for (int i = 2; i < width - 2; i++)
		drawPix(screen, cx++, cy, d);

	drawPix(screen, cx++, cy, b);
	drawPix(screen, cx, cy, b);
	cx = x;
	cy++;

	//row 2
	drawPix(screen, cx++, cy, b);
	drawPix(screen, cx++, cy, d);
	drawPix(screen, cx++, cy, d);

	for (int i = 3; i < width - 3; i++)
		drawPix(screen, cx++, cy, c);

	drawPix(screen, cx++, cy, d);
	drawPix(screen, cx++, cy, d);
	drawPix(screen, cx, cy, b);
	cx = x;
	cy++;

	//row 3
	drawPix(screen, cx++, cy, d);
	drawPix(screen, cx++, cy, d);
	drawPix(screen, cx++, cy, c);

	for (int i = 3; i < width - 3; i++)
		drawPix(screen, cx++, cy, a);

	drawPix(screen, cx++, cy, c);
	drawPix(screen, cx++, cy, d);
	drawPix(screen, cx, cy, d);
	cx = x;
	cy++;

	//row 4
	drawPix(screen, cx++, cy, d);
	drawPix(screen, cx++, cy, c);
	drawPix(screen, cx++, cy, a);

	for (int i = 3; i < width - 3; i++)
		drawPix(screen, cx++, cy, b);

	drawPix(screen, cx++, cy, a);
	drawPix(screen, cx++, cy, c);
	drawPix(screen, cx, cy, d);
	cx = x;
	cy++;

	//sides
	for (int i = 4; i < height - 4; i++)
	{
		drawPix(screen, cx++, cy, d);
		drawPix(screen, cx++, cy, c);
		drawPix(screen, cx++, cy, a);
		drawPix(screen, cx, cy, b);
		cx = x + width - 4;
		drawPix(screen, cx++, cy, b);
		drawPix(screen, cx++, cy, a);
		drawPix(screen, cx++, cy, c);
		drawPix(screen, cx, cy, d);
		cx = x;
		cy++;
	}

	//top
	//row 1
	drawPix(screen, cx++, cy, d);
	drawPix(screen, cx++, cy, c);
	drawPix(screen, cx++, cy, a);

	for (int i = 3; i < width - 3; i++)
		drawPix(screen, cx++, cy, b);

	drawPix(screen, cx++, cy, a);
	drawPix(screen, cx++, cy, c);
	drawPix(screen, cx, cy, d);
	cx = x;
	cy++;

	//row 2
	drawPix(screen, cx++, cy, d);
	drawPix(screen, cx++, cy, d);
	drawPix(screen, cx++, cy, c);

	for (int i = 3; i < width - 3; i++)
		drawPix(screen, cx++, cy, a);

	drawPix(screen, cx++, cy, c);
	drawPix(screen, cx++, cy, d);
	drawPix(screen, cx, cy, d);
	cx = x;
	cy++;

	//row 3
	drawPix(screen, cx++, cy, b);
	drawPix(screen, cx++, cy, d);
	drawPix(screen, cx++, cy, d);

	for (int i = 3; i < width - 3; i++)
		drawPix(screen, cx++, cy, c);

	drawPix(screen, cx++, cy, d);
	drawPix(screen, cx++, cy, d);
	drawPix(screen, cx, cy, b);
	cx = x;
	cy++;

	//row 4
	drawPix(screen, cx++, cy, b);
	drawPix(screen, cx++, cy, b);

	for (int i = 2; i < width - 2; i++)
		drawPix(screen, cx++, cy, d);

	drawPix(screen, cx++, cy, b);
	drawPix(screen, cx++, cy, b);
}

void drawMino(Pix* screen, int x, int y, int type)
{
	int rx = x; //real x and y
	int tx = rx;
	int ry = y;
	int rt = type * 3; //for colour

	if(rx < 0 || rx > WINDOW_WIDTH || ry < 0 || ry > WINDOW_HEIGHT) return; //out of bounds

	if(type == TETRO_VD)
	{
		for(int i = 0; i < CELL_SIZE; i++)
		{
			for(int j = 0; j < CELL_SIZE; j++)
			{
				drawPix(screen, tx++, ry, COLOURS[rt]);
			}
			ry++;
			tx = rx;
		}
		return;
	}

	//blank spaces
	if (type == TETRO_BL)
	{
		//row 1
		for (int i = 0; i < CELL_SIZE; i++)
		{
			drawPix(screen, tx++, ry, COLOURS[rt]);
		}
		tx = rx;
		ry++;

		//row 2-6
		for (int i = 1; i < 6; i++)
		{
			screen[(ry*WINDOW_WIDTH) + tx++] = COLOURS[rt];
			for (int j = 1; j < CELL_SIZE - 1; j++)
			{
				drawPix(screen, tx++, ry, COLOURS[rt + 1]);
			}
			drawPix(screen, tx, ry, COLOURS[rt]);
			tx = rx;
			ry++;
		}

		//row 7-11
		for (int i = 6; i < 11; i++)
		{
			drawPix(screen, tx++, ry, COLOURS[rt]);
			for (int j = 1; j < CELL_SIZE - 1; j++)
			{
				drawPix(screen, tx++, ry, COLOURS[rt + 2]);
			}
			drawPix(screen, tx, ry, COLOURS[rt]);
			tx = rx;
			ry++;
		}

		//row 12
		for (int i = 0; i < CELL_SIZE; i++)
		{
			drawPix(screen, tx++, ry, COLOURS[rt]);
		}
		return;
	}

	//mino blocks
	Pix border;

	border.r = 0.0f;
	border.g = 0.0f;
	border.b = 0.0f;
	border.a = 1.f;

	//row 1
	for (int i = 0; i < CELL_SIZE; i++)
	{
		drawPix(screen, tx++, ry, border);
	}
	tx = rx;
	ry++;

	//row 2
	drawPix(screen, tx++, ry, border);
	for (int i = 1; i < CELL_SIZE - 1; i++)
	{
		drawPix(screen, tx++, ry, COLOURS[rt + 2]);
	}
	drawPix(screen, tx, ry, border);
	tx = rx;
	ry++;

	//row 3
	drawPix(screen, tx++, ry, border);
	for (int i = 1; i < CELL_SIZE - 2; i++)
	{
		drawPix(screen, tx++, ry, COLOURS[rt + 2]);
	}
	drawPix(screen, tx++, ry, COLOURS[rt + 1]);
	drawPix(screen, tx, ry, border);
	tx = rx;
	ry++;

	//row 4-9
	for (int i = 3; i < 9; i++)
	{
		drawPix(screen, tx++, ry, border);
		drawPix(screen, tx++, ry, COLOURS[rt + 2]);
		drawPix(screen, tx++, ry, COLOURS[rt + 2]);
		for (int j = 3; j < CELL_SIZE - 3; j++)
		{
			drawPix(screen, tx++, ry, COLOURS[rt]);
		}
		drawPix(screen, tx++, ry, COLOURS[rt + 1]);
		drawPix(screen, tx++, ry, COLOURS[rt + 1]);
		drawPix(screen, tx, ry, border);
		tx = rx;
		ry++;
	}

	//row 10
	drawPix(screen, tx++, ry, border);
	drawPix(screen, tx++, ry, COLOURS[rt + 2]);
	for (int i = 2; i < CELL_SIZE - 1; i++)
	{
		drawPix(screen, tx++, ry, COLOURS[rt + 1]);
	}
	drawPix(screen, tx, ry, border);
	tx = rx;
	ry++;

	//row 11
	drawPix(screen, tx++, ry, border);
	for (int i = 1; i < CELL_SIZE - 1; i++)
	{
		drawPix(screen, tx++, ry, COLOURS[rt + 1]);
	}
	drawPix(screen, tx, ry, border);
	tx = rx;
	ry++;

	//row 12
	for (int i = 0; i < CELL_SIZE; i++)
	{
		drawPix(screen, tx++, ry, border);
	}
}

void drawTetromino(Pix* screen, Tetromino* tetromino, int xoff, int yoff, int bg)
{
	//draw tetromino
	int tx = tetrominoGetX(tetromino);
	int ty = tetrominoGetY(tetromino);
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (tetrominoGetMino(tetromino, i, j))
			{
				drawMino(screen, CELL_SIZE*(tx + i)+xoff, CELL_SIZE*(ty + j)+yoff, tetrominoGetType(tetromino));
			}
			else if(GFX_MINO_BG_DEBUG)
			{
				drawMino(screen, CELL_SIZE*(tx + i)+xoff, CELL_SIZE*(ty + j)+yoff, TETRO_DB);
			}
			else if(bg) //if we blank out the background tiles
			{
				drawMino(screen, CELL_SIZE*(tx + i)+xoff, CELL_SIZE*(ty + j)+yoff, TETRO_VD);
			}
		}
	}
	if(GFX_MINO_LOCATION_DEBUG)
	{
		Pix c = {0.f, 0.f, 0.f, 0.f};
		drawText(CELL_SIZE*tx+xoff+2, CELL_SIZE*ty+yoff+8, "X", c);
	}
}

void drawText(int x, int y, const char* text, Pix colour)
{
	//just pass it on (only here so we can use it with Pix)
	printFont(x, y, text, colour.r, colour.g, colour.b);
}

void drawLabel(int tb)
{
	//gather some centre justify data
	int x = uiGetX(tb);
	int y = uiGetY(tb);
	int w = uiGetW(tb);
	int h = uiGetH(tb);

	Pix tc = {1.f, 1.f, 1.f, 1.f};
	int tl = strlen(uiGetText(tb));

	int tx = (w / 2) - ((tl / 2) * TEXT_WIDTH);
	tx -= tl % 2 == 1 ? TEXT_WIDTH / 2 : 0; //remove half for if the text has an odd num of chars

	//ty is really finnicky if text sizes change
	int ty = (h / 2) - (TEXT_HEIGHT / 2);

	//background square
	Pix bg = uiGetBG(tb);
	glColor4f(bg.r, bg.g, bg.b, bg.a);
	glBegin(GL_POLYGON);
	glVertex2i(x, y);
	glVertex2i(x + w, y);
	glVertex2i(x + w, y + h);
	glVertex2i(x, y + h);
	glEnd();

	//draw text over top
	drawText(tx + x, ty + y, uiGetText(tb), tc);
}

void drawButton(int tb)
{
	//gather some centre justify data
	int x = uiGetX(tb);
	int y = uiGetY(tb);
	int w = uiGetW(tb);
	int h = uiGetH(tb);

	Pix tc = {1.f, 1.f, 1.f, 1.f};
	int tl = strlen(uiGetText(tb));

	int tx = (w / 2) - ((tl / 2) * TEXT_WIDTH);
	tx -= tl % 2 == 1 ? TEXT_WIDTH / 2 : 0; //remove half for if the text has an odd num of chars

	//ty is really finnicky if text sizes change
	int ty = (h / 2) - (TEXT_HEIGHT / 2);

	//background colour
	Pix bg = uiGetBG(tb);
	if(uiGetState(tb) & BUTTON_CLICK)
	{
		bg.r -= 0.1f;
		bg.g -= 0.1f;
		bg.b -= 0.1f;
	}
	else if(uiGetState(tb) & BUTTON_HOVER)
	{
		bg.r += 0.1f;
		bg.g += 0.1f;
		bg.b += 0.1f;
	}

	//background square
	glColor4f(bg.r, bg.g, bg.b, bg.a);
	glBegin(GL_POLYGON);
	glVertex2i(x, y);
	glVertex2i(x + w, y);
	glVertex2i(x + w, y + h);
	glVertex2i(x, y + h);
	glEnd();

	//draw text over top
	drawText(tx + x, ty + y, uiGetText(tb), tc);
}

void drawSelector(int tb)
{
	//gather some centre justify data
	int x = uiGetX(tb);
	int y = uiGetY(tb);
	int w = uiGetW(tb);
	int h = uiGetH(tb);

	Pix tc = {1.f, 1.f, 1.f, 1.f};
	int tl = strlen(uiGetText(tb));

	//selectors get dynamic data, so we may need to append a ...
	char* text = (char*)calloc(tl+1, sizeof(char));
	if(tl * TEXT_WIDTH > (w-TEXT_WIDTH))
	{
		int ml = (w/TEXT_WIDTH)-4;
		memcpy(text, uiGetText(tb), ml);
		memcpy(&(text[ml]), "...", 3);
		tl = ml+3;
	}
	else
	{
		memcpy(text, uiGetText(tb), tl);
	}

	int tx = (w / 2) - ((tl / 2) * TEXT_WIDTH);
	tx -= tl % 2 == 1 ? TEXT_WIDTH / 2 : 0; //remove half for if the text has an odd num of chars

	//ty is really finnicky if text sizes change
	int ty = (h / 2) - (TEXT_HEIGHT / 2);

	//background colour
	Pix bg = uiGetBG(tb);
	if(uiGetState(tb) & SELECTOR_SELECTED) //brighter for selected
	{
		bg.r += 0.2f;
		bg.g += 0.2f;
		bg.b += 0.2f;
	}
	else if(uiGetState(tb) & SELECTOR_HOVER) //a little brighter for hover
	{
		bg.r += 0.1f;
		bg.g += 0.1f;
		bg.b += 0.1f;
	}
	//unselected will look "transparent" due to same background

	//background square
	glColor4f(bg.r, bg.g, bg.b, bg.a);
	glBegin(GL_POLYGON);
	glVertex2i(x, y);
	glVertex2i(x + w, y);
	glVertex2i(x + w, y + h);
	glVertex2i(x, y + h);
	glEnd();

	//draw text over top
	drawText(tx + x, ty + y, text, tc);
	free(text);
}

void drawTextbox(int tb)
{
	int x = uiGetX(tb);
	int y = uiGetY(tb);
	int w = uiGetW(tb);
	int h = uiGetH(tb);

	//get left justify data
	Pix tc = {1.f, 1.f, 1.f, 1.f};
	int ty = (h / 2) - (TEXT_HEIGHT/2);
	int tx = TEXT_WIDTH / 2;

	//background square
	Pix bg = uiGetBG(tb);
	glColor4f(bg.r, bg.g, bg.b, bg.a);
	glBegin(GL_POLYGON);
	glVertex2i(x, y);
	glVertex2i(x + w, y);
	glVertex2i(x + w, y + h);
	glVertex2i(x, y + h);
	glEnd();

	//draw text over top
	drawText(tx + x, ty + y, uiGetText(tb), tc);

	if(uiGetState(tb) & TEXTBOX_ACTIVE)
		drawText(tx + x + (uiGetTextLength(tb) * TEXT_WIDTH), ty + y, "|", tc); //carat
}

void drawScrollbar(int sb)
{
	if(uiGetScrollGroupSize(sb) >= uiGetScrollDataSize(sb)) return; //don't draw the bar if there's no need to

	//scroll bar
	int x = uiGetX(sb);
	int y = uiGetY(sb);
	int w = uiGetW(sb);
	int h = uiGetH(sb);

	//scroll slider
	int bx = x;
	int by = y;
	int bw = w;
	int bh = h;

	//slider data
	float bar = (float)uiGetScrollDataIndex(sb) / (float)(uiGetScrollDataSize(sb));
	float barSize = (float)uiGetScrollGroupSize(sb) / (float)uiGetScrollDataSize(sb);
	if(uiGetScrollVH(sb)) //horizontal scroll slider
	{
		bx = (x + w) - (bar * w);
		bw = (barSize * w);
		bh = -bh;
	}
	else //vertical scroll slider
	{
		by = (y + h) - (bar * h);
		bh = (barSize * h);
		bw = -bw;
	}

	//draw the scroll bar
	Pix bg = uiGetBG(sb);
	glColor4f(bg.r - 0.1f, bg.g - 0.1f, bg.b - 0.1f, bg.a);
	glBegin(GL_POLYGON);
	glVertex2i(x, y);
	glVertex2i(x + w, y);
	glVertex2i(x + w, y + h);
	glVertex2i(x, y + h);
	glEnd();

	//draw the scroll slider
	glColor4f(bg.r + 0.1f, bg.g + 0.1f, bg.b + 0.1f, bg.a);
	glBegin(GL_POLYGON);
	glVertex2i(bx, by);
	glVertex2i(bx - bw, by);
	glVertex2i(bx - bw, by - bh);
	glVertex2i(bx, by - bh);
	glEnd();
}

void drawBG(Pix bg)
{
	glColor4f(bg.r, bg.g, bg.b, bg.a);
	glBegin(GL_POLYGON);
	glVertex2i(0, 0);
	glVertex2i(0, WINDOW_HEIGHT);
	glVertex2i(WINDOW_WIDTH, WINDOW_HEIGHT);
	glVertex2i(WINDOW_WIDTH, 0);
	glEnd();
}

void drawClearScreen(Pix* screen)
{
	//clear an entire screen
	Pix n = {0.f, 0.f, 0.f, 0.f};
	for(int i = 0; i < WINDOW_WIDTH * WINDOW_HEIGHT; i++)
	{
		screen[i] = n;
	}
}

void drawUI()
{
	int count = uiGetBoxCount();

	//background for non-main ui contexts
	if(uiGetContext() != UI_CONTEXT_MAIN)
	{
		Pix bg = {0.1f, 0.1f, 0.1f, 1.f};
		drawBG(bg);
	}

	for(int i = 0; i < count; i++)
	{
		switch(uiGetType(i))
		{
			case UI_TEXTBOX:
				drawTextbox(i);
				break;
			case UI_BUTTON:
				if(uiGetState(i) & BUTTON_INACTIVE) break;
				drawButton(i);
				break;
			case UI_LABEL:
				drawLabel(i);
				break;
			case UI_SELECTOR:
				if(uiGetState(i) & SELECTOR_INACTIVE) break;
				drawSelector(i);
				break;
			case UI_SCROLLBAR:
				drawScrollbar(i);
				break;
			default:
				//oops!
				break;
		}
	}

	//draw the console if needed
	if(consoleGetEnabled()) drawConsole();
}

void drawConsole()
{
	consoleLine* cl = consoleGetConsole();

	//status colours
	Pix STATUS_COLOUR[] = {
		{1.f, 1.f, 1.f, 1.f},
		{1.f, 1.f, 0.f, 1.f},
		{1.f, 0.f, 0.f, 1.f}
	};

	//console buffer
	glColor4f(0.3f, 0.3f, 0.3f, 0.7f);
	glBegin(GL_POLYGON);
	glVertex2i(0, WINDOW_HEIGHT);
	glVertex2i(WINDOW_WIDTH, WINDOW_HEIGHT);
	glVertex2i(WINDOW_WIDTH, WINDOW_HEIGHT/2 + CELL_SIZE);
	glVertex2i(0, WINDOW_HEIGHT/2 + CELL_SIZE);
	glEnd();

	//work backwards to write the console buffer
	for(int i = 0; i < (WINDOW_HEIGHT/2)/CELL_SIZE; i++)
	{
		if(cl == NULL) break;
		drawText(0, (WINDOW_HEIGHT/2) + (CELL_SIZE * (i+1)), cl->line, STATUS_COLOUR[cl->status]);
		cl = cl->prev;
	}

	//line prompt
	glColor4f(0.2f, 0.2f, 0.2f, 0.7f);
	glBegin(GL_POLYGON);
	glVertex2i(0, WINDOW_HEIGHT/2 + CELL_SIZE);
	glVertex2i(WINDOW_WIDTH, WINDOW_HEIGHT/2 + CELL_SIZE);
	glVertex2i(WINDOW_WIDTH, WINDOW_HEIGHT/2);
	glVertex2i(0, WINDOW_HEIGHT/2);
	glEnd();

	char lineText[LINE_BUF_SIZE + 2];
	sprintf(lineText, "> %s", consoleGetLine());
	drawText(0, WINDOW_HEIGHT/2, lineText, STATUS_COLOUR[CON_MSG]);
}

