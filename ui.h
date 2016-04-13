#ifndef UI_H
#define UI_H

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gvars.h"

#define MAX_BUTTONS 30 //30 buttons max
#define MAX_TEXT 256 //256 characters max for textbox
#define CONTEXT_COUNT 4

typedef struct _textbox {
	int x; //x coord
	int y; //y coord
	int w; //width
	int h; //height
	char* text; //text displayed
	int textLength; //current length of text
	char* name; //name id
	int state; //state (ie: selected, etc)
	int type; //type (ie: textbox, button, etc)
	Pix bg; //background colour of box
	void (*event)(void); //event function called when activated (for use on buttons)
} Textbox;

typedef struct _scrollbar {
	int x; //x coord
	int y; //y coord
	int w; //width
	int h; //height
	int vh; //0 = vertical, 1 = horizontal
	Pix bg; //scrollbar colour
	char* group; //selector group it belongs to
	char** data; //data to use
	int dataIndex; //data index (where we are located)
	int dataSize; //size of data
	int groupSize; //size of group (use with dataIndex and dataSize to create the slider)
	int selectedIndex; //index of selected box
} Scrollbar;

//various ui info
enum {UI_TEXTBOX, UI_BUTTON, UI_LABEL, UI_SELECTOR, UI_SCROLLBAR};
enum {TEXTBOX_HOVER=1, TEXTBOX_ACTIVE=2};
enum {BUTTON_HOVER=1, BUTTON_CLICK=2, BUTTON_INACTIVE=4};
enum {SELECTOR_HOVER=1, SELECTOR_ACTIVE=2, SELECTOR_INACTIVE=4, SELECTOR_SELECTED=8};
enum {UI_CONTEXT_MAIN, UI_CONTEXT_SAVE, UI_CONTEXT_LOAD, UI_CONTEXT_NEW};

void uiInit(); //init ui
void uiDestroy(); //destroy ui
void uiSetContext(int context); //switch contexts
int uiGetContext(); //returns context type

void uiAddLabel(const char* name, int x, int y, int w, int h, Pix bg); //create new label
void uiDelLabel(const char* name); //deletes a label

void uiAddButton(const char* name, int x, int y, int w, int h, Pix bg, void (*event)(void)); //create new button
void uiDelButton(const char* name); //delete button
void uiSetInactiveButton(const char* name); //sets button to inactive (hidden)
void uiSetActiveButton(const char* name); //sets button to active (default state)

void uiAddTextbox(const char* name, int x, int y, int w, int h, Pix bg, const char* text); //create new textbox
void uiDelTextbox(const char* name); //delete textbox
void uiAddCharTextbox(char c); //add character to active textboxes
void uiDelCharTextbox(); //delete character from active textboxes

void uiAddSelector(const char* group, int x, int y, int w, int h, Pix bg); //create new selector button
void uiDelSelector(const char* group); //removes a single selector button
void uiLoadSelectorText(const char* group, char** data, int size); //load string data into selectors text
char* uiGetSelectorText(const char* group); //get the selected text

void uiAddScrollbar(const char* group, int x, int y, int w, int h, Pix bg); //create new scrollbar
void uiDelScrollbar(const char* group); //delete scrollbar
void uiSetDataScrollbar(const char* group, char** data, int size); //sets the data to the scrollbar (calls uiLoadSelectorText too)
void uiScrollUp(int index, int v); //scrolls up in the data v times
void uiScrollDown(int index, int v); //scrolls down in the data v times

void uiCursorEvent(double x, double y); //ui cursor event handler
void uiButtonEvent(int action, int button); //ui cursor button event handler
void uiScrollEvent(double x_offset, double y_offset); //ui scroll event handler

char* uiGetTextboxText(const char* name); //gets text from textbox

//get values (for use in graphics)
int uiGetBoxCount();
char* uiGetText(int i);
int uiGetX(int i);
int uiGetY(int i);
int uiGetW(int i);
int uiGetH(int i);
Pix uiGetBG(int i);
int uiGetTextLength(int i);
int uiGetState(int i);
int uiGetType(int i);
int uiGetScrollVH(int i);
int uiGetScrollDataIndex(int i);
int uiGetScrollDataSize(int i);
int uiGetScrollGroupSize(int i);

#endif

