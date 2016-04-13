#include "ui.h"

//contexts and their respective box lengths
static Textbox*** contexts;
static int* contextLengths;

//context scrollbars
static Scrollbar*** contextScroll;
static int* contextScrollLengths;

//current ui context
static Textbox** ui;
static int* boxLength;

static Scrollbar** scroll;
static int* scrollLength;

//initialise the ui
void uiInit()
{
	//boxes (main context stuff)
	contexts = (Textbox***)calloc(CONTEXT_COUNT, sizeof(Textbox**));
	contextLengths = (int*)calloc(CONTEXT_COUNT, sizeof(int));

	//scroll bars (extra context stuff. even though only load uses this, we do it this way for consistency)
	contextScroll = (Scrollbar***)calloc(CONTEXT_COUNT, sizeof(Scrollbar**));
	contextScrollLengths = (int*)calloc(CONTEXT_COUNT, sizeof(int));
	for(int i = 0; i < CONTEXT_COUNT; i++)
	{
		contexts[i] = (Textbox**)calloc(MAX_BUTTONS, sizeof(Textbox*));
		contextLengths[i] = 0;
		contextScroll[i] = (Scrollbar**)calloc(MAX_BUTTONS, sizeof(Scrollbar*));
		contextScrollLengths[i] = 0;
	}

	uiSetContext(UI_CONTEXT_MAIN);
}

//destroy the ui
void uiDestroy()
{
	//remove everything in it
	for(int i = 0; i < CONTEXT_COUNT; i++)
	{
		uiSetContext(i);
		for(int j = 0; j < *boxLength; j++)
		{
			if(ui[j]->text != NULL) free(ui[j]->text);
			if(ui[j]->name != NULL) free(ui[j]->name);
			free(ui[j]);
		}
		free(ui);
	}
	free(contextLengths);
	free(contexts);
}

//switch contexts
void uiSetContext(int context)
{
	if(context < 0 || context >= CONTEXT_COUNT) return;
	ui = contexts[context];
	boxLength = &(contextLengths[context]);
	scroll = contextScroll[context];
	scrollLength = &(contextScrollLengths[context]);
}

//get which context we're on
int uiGetContext()
{
	for(int i = 0; i < CONTEXT_COUNT; i++)
	{
		if(ui == contexts[i]) return i;
	}
	return -1;
}

//remove a single element at an index for ui boxes (internal function)
void uiRemoveBox(int index)
{
	if(ui[index] == NULL) return;
	if(ui[index]->name != NULL) free(ui[index]->name);
	if(ui[index]->text != NULL) free(ui[index]->text);

	free(ui[index]);
	ui[index] = NULL;

	for(int i = index + 1; i < *boxLength; i++)
	{
		ui[i-1] = ui[i];
	}
	(*boxLength)--;
	ui[*boxLength] = NULL;
}

//remove a single element at an index in scrollbars (internal function)
void uiRemoveScroll(int index)
{
	if(scroll[index] == NULL) return;
	if(scroll[index]->group != NULL) free(scroll[index]->group);

	for(int i = 0; i < scroll[index]->dataSize; i++)
	{
		free(scroll[index]->data[i]);
	}
	if(scroll[index]->data != NULL) free(scroll[index]->data);
	free(scroll[index]);
	scroll[index] = NULL;

	for(int i = index + 1; i < *scrollLength; i++)
	{
		scroll[i-1] = scroll[i];
	}
	(*scrollLength)--;
	scroll[*scrollLength] = NULL;
}

//add label to ui
void uiAddLabel(const char* name, int x, int y, int w, int h, Pix bg)
{
	if(*boxLength >= MAX_BUTTONS) return;

	Textbox* result = (Textbox*)calloc(1, sizeof(Textbox));
	result->x = x;
	result->y = y;
	result->w = w;
	result->h = h;
	result->bg = bg;
	result->type = UI_LABEL;
	result->name = (char*)calloc(strlen(name)+1, sizeof(char));
	memcpy(result->name, name, strlen(name));
	ui[(*boxLength)++] = result;
}

//remove label from ui
void uiDelLabel(const char* name)
{
	for(int i = 0; i < *boxLength; i++)
	{
		if(ui[i]->type == UI_LABEL)
		{
			if(strcmp(name, ui[i]->name) == 0)
			{
				uiRemoveBox(i);
				return;
			}
		}
	}
}

//add button to ui
void uiAddButton(const char* name, int x, int y, int w, int h, Pix bg, void (*event)(void))
{
	if(*boxLength >= MAX_BUTTONS) return;

	Textbox* result = (Textbox*)calloc(1, sizeof(Textbox));
	result->x = x;
	result->y = y;
	result->w = w;
	result->h = h;
	result->bg = bg;
	result->type = UI_BUTTON;
	result->name = (char*)calloc(strlen(name)+1, sizeof(char));
	memcpy(result->name, name, strlen(name));
	result->event = event;
	ui[(*boxLength)++] = result;
}

//remove button from ui
void uiDelButton(const char* name)
{
	for(int i = 0; i < *boxLength; i++)
	{
		if(ui[i]->type == UI_BUTTON)
		{
			if(strcmp(name, ui[i]->name) == 0)
			{
				uiRemoveBox(i);
				return;
			}
		}
	}
}

//sets button to inactive
void uiSetInactiveButton(const char* name)
{
	for(int i = 0; i < *boxLength; i++)
	{
		if(ui[i]->type == UI_BUTTON)
		{
			if(strcmp(name, ui[i]->name) == 0)
			{
				ui[i]->state = BUTTON_INACTIVE;
			}
		}
	}
}

//reactivates button
void uiSetActiveButton(const char* name)
{
	for(int i = 0; i < *boxLength; i++)
	{
		if(ui[i]->type == UI_BUTTON)
		{
			if(strcmp(name, ui[i]->name) == 0)
			{
				ui[i]->state = 0;
			}
		}
	}
}

//add textbox to ui
void uiAddTextbox(const char* name, int x, int y, int w, int h, Pix bg, const char* text)
{
	if(*boxLength >= MAX_BUTTONS) return;

	int len = strlen(text);
	Textbox* result = (Textbox*)calloc(1, sizeof(Textbox));
	result->x = x;
	result->y = y;
	result->w = w;
	result->h = h;
	result->bg = bg;
	result->type = UI_TEXTBOX;
	result->name = (char*)calloc(strlen(name)+1, sizeof(char));
	memcpy(result->name, name, strlen(name));

	//don't let the text be too long (probably don't need this like this)
	result->text = (char*)calloc(MAX_TEXT, sizeof(char));
	if(len > MAX_TEXT)
	{
		memcpy(result->text, text, MAX_TEXT-1);
		result->textLength = MAX_TEXT-1;
	}
	else
	{
		result->textLength = len;
		memcpy(result->text, text, len);
	}

	ui[(*boxLength)++] = result;
}

//remove textbox from ui
void uiDelTextbox(const char* name)
{
	for(int i = 0; i < *boxLength; i++)
	{
		if(ui[i]->type == UI_TEXTBOX)
		{
			if(strcmp(name, ui[i]->name) == 0)
			{
				uiRemoveBox(i);
				return;
			}
		}
	}
}

//add character to all active textboxes
void uiAddCharTextbox(char c)
{
	for(int i = 0; i < *boxLength; i++)
	{
		if(ui[i]->type == UI_TEXTBOX && (ui[i]->state & TEXTBOX_ACTIVE))
		{
			if(ui[i]->textLength >= MAX_TEXT) return; //too long
			ui[i]->text[ui[i]->textLength++] = c;
		}
	}
}

//remove character from all active textboxes
void uiDelCharTextbox()
{
	for(int i = 0; i < *boxLength; i++)
	{
		if(ui[i]->type == UI_TEXTBOX && (ui[i]->state & TEXTBOX_ACTIVE))
		{
			if(ui[i]->textLength <= 0) return; //too short
			ui[i]->textLength--;
			ui[i]->text[ui[i]->textLength] = 0;
		}
	}
}

void uiAddSelector(const char* group, int x, int y, int w, int h, Pix bg)
{
	if(*boxLength >= MAX_BUTTONS) return;

	Textbox* result = (Textbox*)calloc(1, sizeof(Textbox));
	result->x = x;
	result->y = y;
	result->w = w;
	result->h = h;
	result->bg = bg;
	result->type = UI_SELECTOR;
	result->state = SELECTOR_INACTIVE; //default as inactive
	result->name = (char*)calloc(strlen(group)+1, sizeof(char));
	memcpy(result->name, group, strlen(group));

	//update scrollbar
	for(int i = 0; i < *scrollLength; i++)
	{
		if(strcmp(group, scroll[i]->group) == 0)
		{
			scroll[i]->groupSize++;
		}
	}

	ui[(*boxLength)++] = result;
}

void uiDelSelector(const char* group)
{
	for(int i = 0; i < *boxLength; i++)
	{
		if(ui[i]->type == UI_SELECTOR)
		{
			if(strcmp(group, ui[i]->name) == 0)
			{
				//update scrollbar
				for(int j = 0; j < *scrollLength; j++)
				{
					if(strcmp(group, scroll[j]->group) == 0)
					{
						scroll[j]->groupSize--;
						break;
					}
				}

				uiRemoveBox(i);
				return;
			}
		}
	}
}

void uiLoadSelectorText(const char* group, char** data, int size)
{
	int dp = 0;
	for(int i = 0; i < *boxLength; i++)
	{
		if(ui[i]->type == UI_SELECTOR)
		{
			if(dp >= size) //data has been exhausted
			{
				ui[i]->text = NULL;
				ui[i]->state = SELECTOR_INACTIVE;
			}
			else if(strcmp(group, ui[i]->name) == 0) //add it to the list
			{
				ui[i]->text = data[dp++];
				ui[i]->state = SELECTOR_ACTIVE;
			}
		}
	}
}

//grab which selector is selected
char* uiGetSelectorText(const char* group)
{
	for(int i = 0; i < *boxLength; i++)
	{
		if(ui[i]->type == UI_SELECTOR)
		{
			if(strcmp(group, ui[i]->name) == 0)
			{
				if(ui[i]->state & SELECTOR_SELECTED)
				{
					return ui[i]->text;
				}
			}
		}
	}
	return NULL;
}

void uiAddScrollbar(const char* group, int x, int y, int w, int h, Pix bg)
{
	if(*scrollLength >= MAX_BUTTONS) return;

	int groupSize = 0;
	Scrollbar* result = (Scrollbar*)calloc(1, sizeof(Scrollbar));
	result->x = x;
	result->y = y;
	result->w = w;
	result->h = h;
	result->vh = w > h ? 1 : 0; //vertical or horizontal scrollbar (none of these weird wide bars)
	result->group = (char*)calloc(strlen(group)+1, sizeof(char));
	memcpy(result->group, group, strlen(group));
	result->bg = bg;

	//count how many is in this group (implicitly binds the scrollbar to the group)
	for(int i = 0; i < *boxLength; i++)
	{
		if(ui[i]->type == UI_SELECTOR)
		{
			if(strcmp(group, ui[i]->name) == 0)
			{
				groupSize++;
			}
		}
	}

	result->groupSize = groupSize;
	scroll[(*scrollLength)++] = result;
}

void uiDelScrollbar(const char* group)
{
	for(int i = 0; i < *scrollLength; i++)
	{
		if(strcmp(group, scroll[i]->group) == 0)
		{
			uiRemoveScroll(i);
			return;
		}
	}
}

void uiSetDataScrollbar(const char* group, char** data, int size)
{
	for(int i = 0; i < *scrollLength; i++)
	{
		if(strcmp(group, scroll[i]->group) == 0)
		{
			//housekeeping (free existing data)
			if(scroll[i]->data != NULL)
			{
				for(int j = 0; j < scroll[i]->dataSize; j++)
				{
					free(scroll[i]->data[j]);
				}
				free(scroll[i]->data);
			}

			if(data == NULL)
			{
				uiLoadSelectorText(group, NULL, 0); //will set all selectors as inactive
				return;
			}

			scroll[i]->data = (char**)calloc(size, sizeof(char*));
			for(int j = 0; j < size; j++)
			{
				scroll[i]->data[j] = (char*)calloc(strlen(data[j])+1, sizeof(char));
				memcpy(scroll[i]->data[j], data[j], strlen(data[j]));
			}
			scroll[i]->dataSize = size;
			scroll[i]->dataIndex = 0;
			scroll[i]->selectedIndex = 0;
			//load text into the group buttons
			uiLoadSelectorText(group, &(scroll[i]->data[scroll[i]->dataIndex]), scroll[i]->dataSize - scroll[i]->dataIndex);

			//default the selected value as the first one
			for(int j = 0; j < *boxLength; j++)
			{
				if(ui[j]->type == UI_SELECTOR)
				{
					if(strcmp(ui[j]->name, scroll[i]->group) == 0)
					{
						if(strcmp(ui[j]->text, scroll[i]->data[scroll[i]->selectedIndex]) == 0)
						{
							ui[j]->state |= SELECTOR_SELECTED;
							return;
						}
					}
				}
			}
			return;
		}
	}
}

void uiScrollUp(int index, int v)
{
	if(index < 0 || index >= *scrollLength) return;

	for(int i = 0; i < v; i++)
	{
		if(scroll[index]->dataIndex <= 0) break;
		scroll[index]->dataIndex--;
	}

	uiLoadSelectorText(scroll[index]->group, &(scroll[index]->data[scroll[index]->dataIndex]), scroll[index]->dataSize - scroll[index]->dataIndex);

	//this ensures we keep the selected box as we're scrolling
	for(int i = 0; i < *boxLength; i++)
	{
		if(ui[i]->type == UI_SELECTOR)
		{
			if(strcmp(ui[i]->name, scroll[index]->group) == 0)
			{
				if(strcmp(ui[i]->text, scroll[index]->data[scroll[index]->selectedIndex]) == 0)
				{
					ui[i]->state |= SELECTOR_SELECTED;
					return;
				}
			}
		}
	}
}

void uiScrollDown(int index, int v)
{
	if(index < 0 || index >= *scrollLength) return;

	for(int i = 0; i < v; i++)
	{
		if(scroll[index]->dataSize - scroll[index]->dataIndex <= scroll[index]->groupSize) break;
		scroll[index]->dataIndex++;
	}

	uiLoadSelectorText(scroll[index]->group, &(scroll[index]->data[scroll[index]->dataIndex]), scroll[index]->dataSize - scroll[index]->dataIndex);

	//this ensures we keep the selected box as we're scrolling
	for(int i = 0; i < *boxLength; i++)
	{
		if(ui[i]->type == UI_SELECTOR)
		{
			if(strcmp(ui[i]->name, scroll[index]->group) == 0)
			{
				if(strcmp(ui[i]->text, scroll[index]->data[scroll[index]->selectedIndex]) == 0)
				{
					ui[i]->state |= SELECTOR_SELECTED;
					return;
				}
			}
		}
	}
}

//ui cursor event handler
void uiCursorEvent(double x, double y)
{
	y = WINDOW_HEIGHT-y; //reverse y because it's silly

	for(int i = 0; i < *boxLength; i++)
	{
		if(x > ui[i]->x && x < (ui[i]->x + ui[i]->w))
		{
			if(y > ui[i]->y && y < (ui[i]->y + ui[i]->h))
			{
				switch(ui[i]->type)
				{
					case UI_TEXTBOX:
						ui[i]->state |= TEXTBOX_HOVER;
						break;
					case UI_BUTTON:
						if(ui[i]->state == BUTTON_INACTIVE) break;
						ui[i]->state |= BUTTON_HOVER;
						break;
					case UI_LABEL:
						break;
					case UI_SELECTOR:
						if(ui[i]->state == SELECTOR_INACTIVE) break;
						ui[i]->state |= SELECTOR_HOVER;
						break;
					default:
						//how did you even get here????
						break;
				}
				continue;
			}
		}
		switch(ui[i]->type)
		{
			case UI_TEXTBOX:
				ui[i]->state &= TEXTBOX_ACTIVE;
				break;
			case UI_BUTTON:
				if(ui[i]->state == BUTTON_INACTIVE) break;
				if(ui[i]->state == BUTTON_CLICK) break;
				ui[i]->state = 0;
				break;
			case UI_LABEL:
				break;
			case UI_SELECTOR:
				if(ui[i]->state == SELECTOR_INACTIVE) break;
				ui[i]->state &= ~SELECTOR_HOVER;
				break;
			default:
				//something broke
				break;
		}
	}
}

void uiButtonEvent(int action, int button)
{
	if(button != GLFW_MOUSE_BUTTON_LEFT) return;

	if(action == GLFW_PRESS)
	{
		for(int i = 0; i < *boxLength; i++)
		{
			switch(ui[i]->type)
			{
				case UI_TEXTBOX:
					//this makes sure we only have one active textbox at a time
					if(ui[i]->state != TEXTBOX_HOVER)
					{
						ui[i]->state = 0;
						break;
					}
					ui[i]->state = TEXTBOX_ACTIVE;
					break;
				case UI_BUTTON:
					if(ui[i]->state != BUTTON_HOVER) break;
					ui[i]->state |= BUTTON_CLICK; //set the click as well as hover (we'll need it soon)
					break;
				case UI_LABEL:
					break;
				case UI_SELECTOR:
					if(!(ui[i]->state & SELECTOR_HOVER)) break; //if the selector isn't being hovered over, leave
					ui[i]->state |= SELECTOR_SELECTED;
					//unselect the other one (if there was one selected)
					for(int j = 0; j < *boxLength; j++)
					{
						if(j == i) continue;
						if(ui[j]->type == UI_SELECTOR)
						{
							if(strcmp(ui[i]->name, ui[j]->name) == 0)
							{
								ui[j]->state &= ~SELECTOR_SELECTED;
							}
						}
					}
					//set the new selected index for the scrollbar
					for(int j = 0; j < *scrollLength; j++)
					{
						if(strcmp(ui[i]->name, scroll[j]->group) == 0)
						{
							for(int k = 0; k < scroll[j]->dataSize; k++)
							{
								if(strcmp(ui[i]->text, scroll[j]->data[k]) == 0)
								{
									scroll[j]->selectedIndex = k;
								}
							}
						}
					}
					break;
				default:
					//oops!
					break;
			}
		}
	}
	else if(action == GLFW_RELEASE)
	{
		for(int i = 0; i < *boxLength; i++)
		{
			switch(ui[i]->type)
			{
				case UI_TEXTBOX:
					break;
				case UI_BUTTON:
					if(ui[i]->state != (BUTTON_HOVER | BUTTON_CLICK)) break;
					ui[i]->state = 0;
					(ui[i]->event)();
					break;
				case UI_LABEL:
					break;
				case UI_SELECTOR:
					break;
				default:
					//should never happen
					break;
			}
		}
	}
}

void uiScrollEvent(double x_offset, double y_offset)
{
	for(int i = 0; i < *scrollLength; i++)
	{
		if(scroll[i]->vh) //horizontal (use x_offset)
		{
			int x = (int)x_offset;
			if(x < 0)
			{
				uiScrollDown(i, -x);
			}
			else if(x > 0)
			{
				uiScrollUp(i, x);
			}
		}
		else //vertical (use y_offset)
		{
			int y = (int)y_offset;
			if(y < 0)
			{
				uiScrollDown(i, -y);
			}
			else if(y > 0)
			{
				uiScrollUp(i, y);
			}
		}
	}
}

//get text from textbox (save uses this)
char* uiGetTextboxText(const char* name)
{
	for(int i = 0; i < *boxLength; i++)
	{
		if(ui[i]->type == UI_TEXTBOX)
		{
			if(strcmp(name, ui[i]->name) == 0)
			{
				return ui[i]->text;
			}
		}
	}
	return NULL;
}

//the rest of these are for graphics to use as it iterates through
int uiGetBoxCount()
{
	return *boxLength + *scrollLength;
}

char* uiGetText(int i)
{
	if(i < 0 || i >= *boxLength) return NULL;

	switch(ui[i]->type)
	{
		case UI_TEXTBOX:
			return ui[i]->text;
			break;
		case UI_BUTTON:
			return ui[i]->name;
			break;
		case UI_LABEL:
			return ui[i]->name;
			break;
		case UI_SELECTOR:
			return ui[i]->text;
			break;
		default:
			//should never happen
			break;
	}
	return NULL;
}

//these getters will facilitate scrollbar so that it's like it's at the end of the box list
int uiGetX(int i)
{
	if(i < 0) return -1;
	if(i >= *boxLength)
	{
		i -= *boxLength;
		if(i >= *scrollLength) return -1;
		return scroll[i]->x;
	}
	
	return ui[i]->x;
}

int uiGetY(int i)
{
	if(i < 0) return -1;
	if(i >= *boxLength)
	{
		i -= *boxLength;
		if(i >= *scrollLength) return -1;
		return scroll[i]->y;
	}

	return ui[i]->y;
}

int uiGetW(int i)
{
	if(i < 0) return -1;
	if(i >= *boxLength)
	{
		i -= *boxLength;
		if(i >= *scrollLength) return -1;
		return scroll[i]->w;
	}

	return ui[i]->w;
}

int uiGetH(int i)
{
	if(i < 0) return -1;
	if(i >= *boxLength)
	{
		i -= *boxLength;
		if(i >= *scrollLength) return -1;
		return scroll[i]->h;
	}

	return ui[i]->h;
}

Pix uiGetBG(int i)
{
	Pix dummy = {0.f, 0.f, 0.f, 0.f}; //return this if i is bad
	if(i < 0) return dummy;
	if(i >= *boxLength)
	{
		i -= *boxLength;
		if(i >= *scrollLength) return dummy;
		return scroll[i]->bg;
	}

	return ui[i]->bg;
}

int uiGetTextLength(int i)
{
	if(i < 0 || i >= *boxLength) return -1;

	return ui[i]->textLength;
}

int uiGetState(int i)
{
	if(i < 0 || i >= *boxLength) return -1;

	return ui[i]->state;
}

int uiGetType(int i)
{
	if(i < 0) return -1;
	if(i >= *boxLength)
	{
		i -= *boxLength;
		if(i >= *scrollLength) return -1;
		return UI_SCROLLBAR;
	}

	return ui[i]->type;
}

//scroll only getters
int uiGetScrollVH(int i)
{
	if(i < 0) return -1;
	if(i >= *boxLength)
	{
		i -= *boxLength;
		if(i >= *scrollLength) return -1;
		return scroll[i]->vh;
	}

	return -1;
}

int uiGetScrollDataIndex(int i)
{
	if(i < 0) return -1;
	if(i >= *boxLength)
	{
		i -= *boxLength;
		if(i >= *scrollLength) return -1;
		return scroll[i]->dataIndex;
	}

	return -1;
}

int uiGetScrollDataSize(int i)
{
	if(i < 0) return -1;
	if(i >= *boxLength)
	{
		i -= *boxLength;
		if(i >= *scrollLength) return -1;
		return scroll[i]->dataSize;
	}

	return -1;
}

int uiGetScrollGroupSize(int i)
{
	if(i < 0) return -1;
	if(i >= *boxLength)
	{
		i -= *boxLength;
		if(i >= *scrollLength) return -1;
		return scroll[i]->groupSize;
	}

	return -1;
}

