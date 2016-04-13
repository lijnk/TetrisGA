#ifndef TEXT_H
#define TEXT_H
#include <stdio.h>
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>
#include "gvars.h"

void initFont(); //init font
void printFont(int x, int y, const char* text, float r, float g, float b); //draw font on the screen

#endif

