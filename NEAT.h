#ifndef NEAT_H
#define NEAT_H
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "NEATContainers.h"
#include "graphics.h"
#include "gvars.h"
#include "events.h"

//save/load info
#define PBLOCKSIZE 10
#define SPBLOCKSIZE 7
#define GMBLOCKSIZE 4
#define GNBLOCKSIZE 3
#define GNDBLOCKSIZE 2
#define MAGICBLOCK 0x4147 //magic number for files

int neatInit(Pool** pool); //initialises the NEAT system
void neatForceNext(Pool* pool); //times out current genome
void neatTick(Pool** pool, int currentFitness); //ticks the NEAT system
int neatDestroy(Pool** pool); //destroys NEAT system

int neatSetInput(Pool* pool, int* b1, int* b2); //sets the input (from the tetris board)
int neatGetOutput(Pool* pool); //grabs output (for controls)

void neatDraw(Pool* pool); //draws neuron data

void neatSave(Pool* pool, const char* filename); //saving pool data
void neatLoad(Pool** pool, const char* filename); //loading pool data. takes double pointer due to writing an entirely new pool

size_t neatSizeof(); //returns sizeof pool

#endif
