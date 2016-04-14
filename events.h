#ifndef EVENTS_H
#define EVENTS_H

#include <dirent.h>
#include "game.h"
#include "NEAT.h"

//main driver events
//This is a separate layer and not in main due to various things needing proper access to these
void eventInit(); //initialise everything
void eventDestroy(); //destroy everything

void eventNewButton(); //new button
void eventNew(); //new command
void eventNewSave(); //save command for new
void eventNewCancel(); //cancel command for new

void eventSave(const char* filename); //save command
void eventSaveButton(); //save button
void eventSaveSaveButton(); //save button inside save screen

void eventLoad(const char* filename); //load command
void eventLoadButton(); //load button
void eventLoadLoadButton(); //load button inside load screen
void eventGetLoadInfo(const char* dir, char*** data, int* size); //grabs load info data

void eventBest(); //does nothing. dummy for now

void eventQuit(); //quit the game

void eventPause(); //pause the game
void eventUnpause(); //unpause the game

void eventCancelButton(); //cancel button

int eventDraw(); //our draw calls. returns 1 if it draw something
void eventTick(); //tick the game
void eventController(); //let the NEAT system control the game
int eventIsRunning(); //check if the system is on
void eventToggleDraw(); //toggles whether we are drawing
int eventGetDraw(); //check whether toggle draw is on or off

#endif

