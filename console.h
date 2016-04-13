#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CONSOLE_BUF_SIZE 128 //this is how many lines, not how many characters
#define LINE_BUF_SIZE 256 //this is actually how many characters

enum {CON_MSG, CON_WARN, CON_ERR}; //console statuses

//double linked list
struct _consoleLine {
	char* line; //line
	int length; //length of line
	int status; //status of line (msg, warn, err)
	struct _consoleLine* next; //next node
	struct _consoleLine* prev; //previous node
};

typedef struct _consoleLine consoleLine;

void consoleInit(); //initialises console
void consoleDestroy(); //destroys console
void consoleClear(); //clears console
void consolePrint(const char* text, int status); //prints stuff into console buffer
void consoleLineAddChar(char c); //adds character to line buffer (event command)
void consoleLineDelChar(); //deletes character from line buffer (event command)
void consoleEnterLine(); //event for enter button (enter the line into console)
void consoleScroll(int step); //scrolls in the buffer

void consoleToggleEnabled(); //toggles if console is enabled
int consoleGetEnabled(); //returns if console is enabled

char* consoleGetLine(); //return line
int consoleGetLineLength(); //returns length of line
consoleLine* consoleGetConsole(); //return console

#endif

