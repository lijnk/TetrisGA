#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include "graphics.h"
#include "console.h"
#include "events.h"

static int fpscap;

//No idea what we'll do here, but for now, let's keep it in
static void error_callback(int error, const char* description)
{
}

//this does stuff (supposed to be for resizing, but now is required for textures to actually show up)
static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.f, width, 0.f, height);
}

//these are for updating hover states
static void cursor_position_callback(GLFWwindow* window, double x, double y)
{
	uiCursorEvent(x, y);
	drawSetReq();
}

//this is to scroll the console or ui
static void scroll_callback(GLFWwindow* window, double x_offset, double y_offset)
{
	consoleScroll((int)y_offset);

	if(!consoleGetEnabled()) //don't scroll if the console is on
		uiScrollEvent(x_offset, y_offset);

	drawSetReq();
}

//these are the ui mouse click events
static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	uiButtonEvent(action, button);
	drawSetReq();
}

//strdup because it's not in string.h
char* strdup(const char* str)
{
  size_t len = strlen(str);
  char* x = (char*)malloc(len+1);
  if(!x) return NULL;
  memcpy(x, str, len+1);
  return x;
}

//splits the command into a simple argv, argc format I can use later on
void splitCMD(const char* input, char*** params, int* pcount)
{
  int count = 0;
  char* in = strdup(input);

	//split on spaces
  char* ptr = strtok(in, " ");
  while(ptr != NULL)
  {
    count++;
    ptr = strtok(NULL, " ");
  }

	//plug them into the parameters
  *params = (char**)calloc(count, sizeof(char*));
  count = 0;
  free(in);
  in = strdup(input);
  ptr = strtok(in, " ");
  while(ptr != NULL)
  {
    (*params)[count++] = strdup(ptr);
    ptr = strtok(NULL, " ");
  }
  *pcount = count;
  free(in);
}

//this used to be in console.c, but circular dependencies doesn't like it there (probably better here)
void parseCommand()
{
	if(!consoleGetEnabled()) return;
	char* printcmd = (char*)calloc(strlen(consoleGetLine()) + 3, CON_MSG);
	sprintf(printcmd, "> %s", consoleGetLine());
	consolePrint(printcmd, CON_MSG);
	free(printcmd);

	if(consoleGetLineLength() == 0) return;

	char** argv;
	int argc;
	splitCMD(consoleGetLine(), &argv, &argc);

	//commands and their respective events
	if(strcmp(argv[0], "quit") == 0)
	{
		eventQuit();
	}
	else if(strcmp(argv[0], "pause") == 0)
	{
		eventPause();
	}
	else if(strcmp(argv[0], "unpause") == 0)
	{
		eventUnpause();
	}
	else if(strcmp(argv[0], "save") == 0)
	{
		if(argc == 2)
		{
			eventSave(argv[1]);
		}
		else
		{
			consolePrint("Usage: save <filename>", CON_ERR);
		}
	}
	else if(strcmp(argv[0], "load") == 0)
	{
		if(argc == 2)
		{
			eventLoad(argv[1]);
		}
		else
		{
			consolePrint("Usage: load <filename>", CON_ERR);
		}
	}
	else if(strcmp(argv[0], "new") == 0)
	{
		eventNew();
	}
	else if(strcmp(argv[0], "clear") == 0)
	{
		consoleClear();
	}
	else if(strcmp(argv[0], "toggledraw") == 0)
	{
		if(argc == 1)
		{
			eventToggleDraw();
		}
	}
	else if(strcmp(argv[0], "fpscap") == 0)
	{
		char msg[20];
		if(argc == 1)
		{
			if(fpscap <= 0)
			{
				consolePrint("fpscap = unlimited", CON_MSG);
			}
			else
			{
				sprintf(msg, "fpscap = %d", fpscap);
				consolePrint(msg, CON_MSG);
			}
		}
		else if(argc == 2)
		{
			fpscap = (float)atoi(argv[1]);
			if(fpscap < 0) fpscap = 0;
		}
		else
		{
			consolePrint("Usage: fpscap [value]", CON_ERR);
		}
	}
	else if(strcmp(argv[0], "help") == 0 && (argc == 1))
	{
		consolePrint("*** Usage ***", CON_MSG);
		consolePrint("pause - Pauses game", CON_MSG);
		consolePrint("unpause - Unpauses game", CON_MSG);
		consolePrint("save <filename> - Saves game", CON_MSG);
		consolePrint("load <filename> - Loads game", CON_MSG);
		consolePrint("new - Reinitialises game", CON_MSG);
		consolePrint("clear - Clears console", CON_MSG);
		consolePrint("toggledraw - Toggles drawing game and simulation on screen", CON_MSG);
		consolePrint("fpscap [value] - Sets or reports fps cap (0 for unlimited)", CON_MSG);
		consolePrint("quit - Quits game", CON_MSG);
	}
	else
	{
		consolePrint("Unknown Command (type 'help' for help)", CON_ERR);
	}

	//memory management
	for(int i = 0; i < argc; i++)
	{
		free(argv[i]);
	}
	free(argv);
}

//Basically handles the console stuff
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	//We'll keep these here in case we want to allow the user to play the game again
	//pressed
	//left - move left
	//right - move right
	//up - turn
	//down - force tick
	//space - drop
	//esc - quit

	drawSetReq();
	if(action == GLFW_PRESS)
	{
		if(key == GLFW_KEY_GRAVE_ACCENT)
		{
			consoleToggleEnabled(); //console
			return;
		}
	}

	if(action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		if(key == GLFW_KEY_BACKSPACE || key == GLFW_KEY_DELETE)
		{
			consoleLineDelChar();
			uiDelCharTextbox();
		}
		else if(key == GLFW_KEY_ENTER)
		{
			parseCommand();
			consoleEnterLine();
		}
		else if(key >= 32 && key <= 126)
		{
			//capitalisation
			if((key >= GLFW_KEY_A && key <= GLFW_KEY_Z) && !(mods & GLFW_MOD_SHIFT)) key += 32;

			//shift key
			if(mods & GLFW_MOD_SHIFT)
			{
				switch(key)
				{
					case '1':
						key = '!';
						break;
					case '2':
						key = '@';
						break;
					case '3':
						key = '#';
						break;
					case '4':
						key = '$';
						break;
					case '5':
						key = '%';
						break;
					case '6':
						key = '^';
						break;
					case '7':
						key = '&';
						break;
					case '8':
						key = '*';
						break;
					case '9':
						key = '(';
						break;
					case '0':
						key = ')';
						break;
					case '`':
						key = '~';
						break;
					case '[':
						key = '{';
						break;
					case ']':
						key = '}';
						break;
					case ';':
						key = ':';
						break;
					case '\'':
						key = '\"';
						break;
					case '\\':
						key = '|';
						break;
					case ',':
						key = '<';
						break;
					case '.':
						key = '>';
						break;
					case '/':
						key = '?';
						break;
				}
			}
			consoleLineAddChar(key);
			uiAddCharTextbox(key);
		}
	}
}

int main(int argc, char** argv)
{
	GLFWwindow* window;
	int width, height;
	fpscap = 60;

	//setup glfw stuff
	glfwSetErrorCallback(error_callback);

	if(!glfwInit())
		exit(EXIT_FAILURE);

	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Tetris GA", NULL, NULL);
	if(!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	//callbacks
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//some more glfw silliness
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	glfwGetFramebufferSize(window, &width, &height);
	framebuffer_size_callback(window, width, height);

	//start game
	eventInit();
	graphicsInit();
	consoleInit();
	consolePrint("******************", CON_MSG);
	consolePrint("*** Tetris  GA ***", CON_MSG);
	consolePrint("******************", CON_MSG);

	//for transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0, 0, 0, 0);

	//autoload a file if we aren't loading graphics
	if(GRAPHICS_HEADLESS)
	{
		eventLoad("tempbk.dat");
	}

	eventPause(); //start paused

	while(!glfwWindowShouldClose(window) && eventIsRunning())
	{
		//fps cap (default 60)
		double curr = glfwGetTime();
		double elapse = 0;

		if(fpscap > 0)
		{
			while(elapse < (1.0 / fpscap))
			{
				elapse = glfwGetTime() - curr;
			}
		}

		//logic
		eventTick();

		//graphics
		//glClear(GL_COLOR_BUFFER_BIT);
		if(eventDraw()) glfwSwapBuffers(window);

		//events
		glfwPollEvents();
	}

	//cleanup
	eventDestroy();
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

