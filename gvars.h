#ifndef GVARS_H
#define GVARS_H

//A whole lot of variables that have to do with measurements of everything

#define GRAPHICS_HEADLESS 0

#define GFX_MINO_LOCATION_DEBUG 0
#define GFX_MINO_BG_DEBUG 0

#define CELL_SIZE 12
#define BORDER 8

//this is for the full window
#define WINDOW_WIDTH (BORDER*2 + 28*CELL_SIZE)
#define WINDOW_HEIGHT (BORDER*2 + 28*CELL_SIZE)

//these are for UI element locations
#define GB_X (CELL_SIZE)
#define GB_Y (CELL_SIZE)
#define GB_WIDTH ((10*CELL_SIZE)+BORDER)
#define GB_HEIGHT ((18*CELL_SIZE)+BORDER)

#define STATS_X ((GB_X+GB_WIDTH)+(CELL_SIZE))
#define STATS_Y ((GB_Y+GB_HEIGHT)-(CELL_SIZE*5))
#define STATS_WIDTH (CELL_SIZE*10)
#define STATS_HEIGHT (CELL_SIZE*5)

#define MINO_X ((GB_X+GB_WIDTH)+(CELL_SIZE*3))
#define MINO_Y ((GB_Y+GB_HEIGHT)-(CELL_SIZE*12))
#define MINO_WIDTH (CELL_SIZE*5)
#define MINO_HEIGHT (CELL_SIZE*5)

#define NET_X (CELL_SIZE*2)
#define NET_Y (WINDOW_HEIGHT - CELL_SIZE*8)
#define NET_WIDTH (CELL_SIZE*20)
#define NET_HEIGHT (CELL_SIZE*6)

#define UI_BUTTON_W ((8*8)+8) //8 for largest word, 8 for text width and 8 for padding
#define UI_BUTTON_H (8+8) //8 for text height, 8 for padding

#define NEW_BUTTON_X (STATS_X + STATS_WIDTH)
#define NEW_BUTTON_Y (MINO_Y+24)

#define SAVE_BUTTON_X (NEW_BUTTON_X)
#define SAVE_BUTTON_Y ((NEW_BUTTON_Y) - UI_BUTTON_H - 4)

#define LOAD_BUTTON_X (NEW_BUTTON_X)
#define LOAD_BUTTON_Y ((SAVE_BUTTON_Y) - UI_BUTTON_H - 4)

#define PAUSE_BUTTON_X (NEW_BUTTON_X)
#define PAUSE_BUTTON_Y ((LOAD_BUTTON_Y) - UI_BUTTON_H - 4)

#define BEST_BUTTON_X (NEW_BUTTON_X)
#define BEST_BUTTON_Y ((PAUSE_BUTTON_Y) - UI_BUTTON_H - 4)

#define QUIT_BUTTON_X (BEST_BUTTON_X)
#define QUIT_BUTTON_Y ((BEST_BUTTON_Y) - UI_BUTTON_H - 4)

#define SAVELOAD_BUTTON_X ((WINDOW_WIDTH/2) - UI_BUTTON_W - 4)
#define SAVELOAD_BUTTON_Y (CELL_SIZE*3)

#define CANCEL_BUTTON_X ((WINDOW_WIDTH/2) + 4)
#define CANCEL_BUTTON_Y (SAVELOAD_BUTTON_Y)

#define SAVELOAD_BANNER_Y (NET_Y+NET_HEIGHT-UI_BUTTON_H)

#define SAVELOAD_DATA_X (CELL_SIZE*3)
#define SAVELOAD_DATA_Y (SAVELOAD_BANNER_Y - (CELL_SIZE*2))
#define SAVELOAD_DATA_W (WINDOW_WIDTH - (SAVELOAD_DATA_X*2))
#define SAVELOAD_DATA_H (UI_BUTTON_H)

#define LOADSCROLL_X (WINDOW_WIDTH - CELL_SIZE*2)
#define LOADSCROLL_Y (SAVELOAD_DATA_Y - ((UI_BUTTON_H + 4) * 7))
#define LOADSCROLL_W 12
#define LOADSCROLL_H (((UI_BUTTON_H + 4) * 8) - 4)

//text info
#define TEXT_HEIGHT 12
#define TEXT_WIDTH 8
#define FONTSHEET_HEIGHT 8
#define FONTSHEET_WIDTH 16

//we need this because opengl is silly and doesn't define half its own library
#define GL_CLAMP_TO_EDGE 0x812F

typedef struct P {
	float r;
	float g;
	float b;
	float a;
} Pix;

#endif

