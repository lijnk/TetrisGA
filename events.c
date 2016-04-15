#include "events.h"

//the major player variables
static Game* game;
static Pool* pool;
static int running;
static int globalTick;
static int draw;
static int pause;

void eventInit()
{
	//initialise important vars
	globalTick = 0;
	running = 1;
	draw = 1;
	pause = 1;

	//create the sim space
	game = (Game*)malloc(gameSizeof());
	neatInitMemInfo();
	gameInit(game);
	neatInit(&pool);
}

void eventDestroy()
{
	//break everything
	gameClean(game);
	free(game);

	neatDestroy(&pool);
	free(pool);

	consoleDestroy();
	uiDestroy();
}

void eventNewButton()
{
	//event for new button
	eventPause();
	uiSetContext(UI_CONTEXT_NEW);
	drawSetReq(DRAW_UI);
}

void eventNew()
{
	//destroys NEAT system and creates a new one
	neatDestroy(&pool);
	neatMemReport();
	neatInit(&pool);
	gameRestart(game);
	eventPause();
	drawSetReq(DRAW_UI);
}

void eventNewSave()
{
	//event for the save button within the "new" screen
	eventSave(uiGetTextboxText("savefn"));
	uiSetContext(UI_CONTEXT_MAIN);
	eventNew();
	drawSetReq(DRAW_UI);
}

void eventNewCancel()
{
	//event for discard button within the "new" screen
	uiSetContext(UI_CONTEXT_MAIN);
	eventNew();
	drawSetReq(DRAW_UI);
}

void eventSave(const char* filename)
{
	//save the game
	neatSave(pool, filename);
}

void eventSaveButton()
{
	//save button on the main screen
	eventPause();
	uiSetContext(UI_CONTEXT_SAVE);
	drawSetReq(DRAW_UI);
}

void eventSaveSaveButton()
{
	//saves system
	char* filename = uiGetTextboxText("savefn");
	int appended = 0;
	if(strstr(filename, ".dat") == NULL)
	{
		//we need the .dat!!!!!
		appended = 1;
		int len = strlen(filename);
		char* temp = (char*)calloc(len + 5, sizeof(char));
		memcpy(temp, filename, len);
		memcpy(temp+len, ".dat", 4);
		filename = temp;
	}
	//save it
	eventSave(filename);
	uiSetContext(UI_CONTEXT_MAIN);
	if(appended) free(filename);
	drawSetReq(DRAW_UI);
}

void eventLoad(const char* filename)
{
	//load a file
	int cc = uiGetContext();
	neatLoad(&pool, filename);
	gameRestart(game);
	uiSetContext(UI_CONTEXT_MAIN); //this weirdness is so the console can call this without worrying about contexts
	eventPause();
	uiSetContext(cc);
}

void eventLoadButton()
{
	//load button on the main screen
	char** data = NULL;
	int size = 0;
	eventGetLoadInfo("./", &data, &size); //grab file info (text on the buttons)

	eventPause();
	uiSetContext(UI_CONTEXT_LOAD);
	
	if(data != NULL)
		uiSetDataScrollbar("loadfn", data, size); //set the buttons up

	for(int i = 0; i < size; i++)
	{
		free(data[i]);
	}
	free(data);
	drawSetReq(DRAW_UI);
}

void eventLoadLoadButton()
{
	//get the filename from the text
	char* filename = uiGetSelectorText("loadfn");
	if(filename != NULL) //let it fall through if it's null. the load system will deal with it
	{
		char* tp = strstr(filename, ".dat");
		tp[4] = 0;
	}

	//load the system and switch contexts back to main
	eventLoad(filename);
	uiSetContext(UI_CONTEXT_MAIN);
	drawSetReq(DRAW_UI);
}

void eventGetLoadInfo(const char* dir, char*** data, int* size)
{
	struct dirent* path;
	FILE* fp;
	char** result = (char**)calloc(100, sizeof(char*)); //100 as max files to grab. might make this dynamic in the future
	int resultLength = 0;

	DIR *dp = opendir(dir);
	if(dp == NULL)
	{
		free(result);
		return;
	}

	while(path = readdir(dp))
	{
		unsigned int magic;
		unsigned int gen;
		if(resultLength >= 100) break;
		if(strstr(path->d_name, ".dat") == NULL) continue; //need the .dat file extension

		fp = fopen(path->d_name, "rb");
		fread(&magic, sizeof(unsigned int), 1, fp);
		if(magic != MAGICBLOCK) //check for the magic number in the file
		{
			fclose(fp);
			continue;
		}

		fseek(fp, 8*sizeof(unsigned int), SEEK_SET);
		fread(&gen, sizeof(unsigned int), 1, fp);
		fclose(fp);

		//give new entry
		result[resultLength] = (char*)calloc(strlen(path->d_name) + sizeof(unsigned int) + 7, sizeof(char)); //size of filename, 4 bytes for gen and string
		sprintf(result[resultLength], "%s - Gen %d", path->d_name, gen);
		resultLength++;
	}

	if(resultLength <= 0)
	{
		free(result);
		return;
	}

	*size = resultLength;
	*data = result;
}

void eventBest()
{
	//just a dummy function right now
}

void eventQuit()
{
	//quit everything
	running = 0;
}

void eventPause()
{
	//pause game (swaps pause/unpause buttons)
	uiSetInactiveButton("Pause");
	uiSetActiveButton("Unpause");
	pause = 1;
	//gamePause(game);
	consolePrint("*** Paused ***", CON_WARN);
	drawSetReq(DRAW_UI | DRAW_CONSOLE);
}

void eventUnpause()
{
	//unpause game (swaps pause/unpause buttons)
	uiSetInactiveButton("Unpause");
	uiSetActiveButton("Pause");
	pause = 0;
	//gameUnpause(game);
	consolePrint("*** Unpaused ***", CON_WARN);
	drawSetReq(DRAW_UI | DRAW_CONSOLE);
}

void eventCancelButton()
{
	uiSetContext(UI_CONTEXT_MAIN);
	drawSetReq(DRAW_UI);
}

int eventDraw()
{
	//our draw stuff
	if(GRAPHICS_HEADLESS) return 0; //turn this on in gvars.h to go super fast
	if(!drawGetReq()) return 0; //so we don't draw a million times per second

	if(!draw)
	{
		if(!(drawGetReq() & DRAW_CONSOLE)) return 0;
	
		glClear(GL_COLOR_BUFFER_BIT);
		Pix bg = {0.1f, 0.1f, 0.1f, 1.f};
		drawBG(bg);
		drawConsole();
		drawResetReq();
		return 1;
	}

	glClear(GL_COLOR_BUFFER_BIT);
	//internal drawing stuff
	gameDraw(game);
	neatDraw(pool);

	//actual drawing stuff
	int cc = uiGetContext();
	if(cc == UI_CONTEXT_MAIN)
	{
		glDrawPixels(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_FLOAT, gameGetBG(game));
		glDrawPixels(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_FLOAT, gameGetFG1(game));
		glDrawPixels(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_FLOAT, gameGetFG2(game));
	}
	drawUI();
	drawResetReq();
	return 1;
}

void eventTick()
{
	//a single tick
	if(pause) return;;

	gameTick(game);
	if(gameGetGameover(game)) neatForceNext(pool);
	if(globalTick % 5 == 0) //every 5 ticks so we don't spam the NEAT system unnecessarily
	{
		int ts1[(10*18)]; //tileset
		int ts2[(10*18)]; //extended tileset

		//set the inputs for the simulation
		for(int i = 0; i < 10; i++)
		{
			for(int j = 0; j < 18; j++)
			{
				int tile = gameGetTile(game, i, j);
				int etile = gameGetExtendedTile(game, i, j);
				ts1[(j*10)+i] = tile;
				ts2[(j*10)+i] = etile;
			}
		}
		neatSetInput(pool, ts1, ts2);
		neatTick(&pool, gameGetScore(game)); //tick the game
		eventController();
	}
	globalTick++;
}

void eventController()
{
	//medium for the NEAT system to access the game
	int controls = neatGetOutput(pool);

  if(controls == -1)
  {
    gameEvent(game, GAME_RESTART);
    return;
  }

  //don't allow left and right pressing
  if(controls & 0x1 && !(controls & 0x2)) gameEvent(game, GAME_LEFT);
  if(controls & 0x2 && !(controls & 0x1)) gameEvent(game, GAME_RIGHT);

  if(controls & 0x4) gameEvent(game, GAME_DOWN);
  if(controls & 0x8) gameEvent(game, GAME_ROTATE);
  if(controls & 0x10) gameEvent(game, GAME_DROP);
}

void eventToggleDraw()
{
	draw ^= 1;
}

int eventGetDraw()
{
	return draw;
}

int eventIsRunning()
{
	//is this thing on???
	return running;
}

