#include "console.h"

static consoleLine* consoleBuffer; //newest line
static consoleLine* firstLine; //oldest line (mostly for memory management)
static consoleLine* lastLine; //where are we in the scroll buffer
static int consoleBufferLength; //length of the console buffer

static char* lineBuffer; //current line
static int lineBufferLength; //length of the line buffer

static int consoleEnabled; //whether the console is turned on

void consoleLineNew(consoleLine** cl, const char* text, int status)
{
	//creates entirely new console line
	*cl = (consoleLine*)malloc(sizeof(consoleLine));
	(*cl)->length = strlen(text);
	(*cl)->line = (char*)malloc(((*cl)->length + 1) * sizeof(char));
	memcpy((*cl)->line, text, (*cl)->length);
	(*cl)->line[(*cl)->length] = 0;
	(*cl)->status = status;
	(*cl)->next = NULL;
	(*cl)->prev = NULL;
}

void consoleLineDel(consoleLine** cl)
{
	//deletes line (we should only be deleting lines at the "beginning")
	if((*cl)->next != NULL) (*cl)->next->prev = NULL;
	free((*cl)->line);
	free(*cl);
	*cl = NULL;
}

void consoleAddLine(const char* text, int status)
{
	//create new line and append it
	consoleLine* nl;
	consoleLineNew(&nl, text, status);
	nl->prev = consoleBuffer;

	//if there's no lines at all
	if(consoleBuffer != NULL) consoleBuffer->next = nl;

	//if our console is scrolled to the bottom, then let it follow
	if(lastLine == consoleBuffer) lastLine = nl;

	consoleBuffer = nl;
	consoleBufferLength++;

	//if there's no lines on the console
	if(firstLine == NULL)
	{
		firstLine = nl;
		lastLine = nl;
	}
	else if(consoleBufferLength > CONSOLE_BUF_SIZE) //if there's too many lines on the console, remove from the end
	{
		if(lastLine == firstLine) lastLine = lastLine->next; //auto-scroll down
		consoleLine* temp = firstLine;
		firstLine = firstLine->next;

		consoleLineDel(&temp);
		consoleBufferLength = CONSOLE_BUF_SIZE;
	}
}

//clears entire console (user command or cleanup will call this)
void consoleClear()
{
	if(consoleBufferLength <= 0) return; //don't do anything if we're already empty

	//clean house!
	consoleLine* temp;

	firstLine = NULL;
	lastLine = NULL;
	for(int i = 0; i < consoleBufferLength; i++)
	{
		temp = consoleBuffer;
		consoleBuffer = consoleBuffer->prev;
		consoleLineDel(&temp);
	}
	consoleBufferLength = 0;
}

void consoleInit()
{
	//init the values
	lineBuffer = (char*)calloc(LINE_BUF_SIZE, sizeof(char)); //256 char for each line (should be high enough)
	lineBufferLength = 0;
	consoleBuffer = NULL;
	firstLine = NULL;
	lastLine = NULL;
	consoleBufferLength = 0;
	consoleEnabled = 0;
}

void consoleDestroy()
{
	//tear it all down
	free(lineBuffer);
	consoleClear();
}

void consolePrint(const char* text, int status)
{
	//print to STDOUT and to our own console
	printf("%s\n", text);
	consoleAddLine(text, status);
}

void consoleLineAddChar(char c)
{
	//what happens when we press a button
	if(!consoleEnabled) return; //return if the console isn't on
	if(lineBufferLength >= LINE_BUF_SIZE) return; //if the user is facerolling the keyboard for some reason

	lineBuffer[lineBufferLength++] = c;
}

void consoleLineDelChar()
{
	//backspace
	if(!consoleEnabled) return;
	if(lineBufferLength <= 0) return; //so we don't delete anything we shouldn't

	lineBufferLength--;
	lineBuffer[lineBufferLength] = 0;
}

void consoleEnterLine()
{
	//basically just erase the line (the enter key)
	//this used to parse as well, but we moved that to main due to circular dependency issues (probably better there anyways)
	if(!consoleEnabled) return;
	if(lineBufferLength <= 0) return;

	//erase line
	for(int i = 0; i < lineBufferLength; i++)
	{
		lineBuffer[i] = 0;
	}
	lineBufferLength = 0;
}

void consoleScroll(int step)
{
	//the scroll event. + numbers is up, - is down
	if(!consoleEnabled) return;
	if(lastLine == NULL) return;

	if(step > 0)
	{
		for(int i = 0; i < step; i++)
		{
			if(lastLine->prev == NULL) return;
			lastLine = lastLine->prev;
		}
	}
	else if(step < 0)
	{
		for(int i = 0; i > step; i--)
		{
			if(lastLine->next == NULL) return;
			lastLine = lastLine->next;
		}
	}
}

void consoleToggleEnabled()
{
	//toggle with an xor
	consoleEnabled ^= 1;
}

int consoleGetEnabled()
{
	return consoleEnabled;
}

char* consoleGetLine()
{
	return lineBuffer;
}

int consoleGetLineLength()
{
	return lineBufferLength;
}

consoleLine* consoleGetConsole()
{
	return lastLine;
}

