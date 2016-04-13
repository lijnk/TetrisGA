#include "text.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static GLuint font_tex;

void initFont()
{
	int w, h, c;
	GLubyte* i;

	//load the png image
	if((i = stbi_load("font.png", &w, &h, &c, STBI_rgb_alpha)) == NULL)
	{
		printf("Error Loading: font.png\n");
		return;
	}

	//make a texture out of it
	glGenTextures(1, &font_tex);
	glBindTexture(GL_TEXTURE_2D, font_tex);

	//some texture parameters that are useful to have
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//set the texture up
	glTexImage2D(GL_TEXTURE_2D, 0, 4, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, i);
	glShadeModel(GL_FLAT);
}

void printFont(int x, int y, const char* text, float r, float g, float b)
{
	//print text from the texture
	int rx = x;
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, font_tex);

	//we might use the alpha channel in the future, for now, we don't need to
	glColor4f(r, g, b, 1.f);
	for(int i = 0; text[i] != 0; i++)
	{
		char c = text[i] % 128; //so if we feed this a weird character, we don't crash
		//grab some texture locations
		GLfloat tx1 = (c % FONTSHEET_WIDTH) / (float)FONTSHEET_WIDTH;
		GLfloat tx2 = ((c % FONTSHEET_WIDTH) + 1) / (float)FONTSHEET_WIDTH;
		GLfloat ty1 = (c / FONTSHEET_WIDTH) / (float)FONTSHEET_HEIGHT;
		GLfloat ty2 = ((c / FONTSHEET_WIDTH) + 1) / (float)FONTSHEET_HEIGHT;

		//draw it
		glBegin(GL_POLYGON);
		glTexCoord2d(tx1, ty1); glVertex2i(rx, y+TEXT_HEIGHT);
		glTexCoord2d(tx2, ty1); glVertex2i(rx+TEXT_WIDTH, y+TEXT_HEIGHT);
		glTexCoord2d(tx2, ty2); glVertex2i(rx+TEXT_WIDTH, y);
		glTexCoord2d(tx1, ty2); glVertex2i(rx, y);
		glEnd();
		rx += TEXT_WIDTH;
	}
	glDisable(GL_TEXTURE_2D);
}

