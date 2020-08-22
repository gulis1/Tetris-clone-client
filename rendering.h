#pragma once
#include<glew.h>
#include <glfw3.h>
#include <map>
#include <fstream>
#include<stb_image.h>

#include "shaderProgram.h"
#include "ficha.h"
#include "Board.h"
#include "color.h"

using namespace std;

typedef struct {
	float x;
	float y;
	float width;
	float height;
	float xoffset;
	float yoffset;
	float xadvance;
}tChar;

typedef map<int, tChar> tFontData;

typedef struct {
	GLuint id;
	GLint width, height;
}tTexture;

class tRenderer {
	public:
		tRenderer();
		void renderText(const string text, GLfloat startX, GLfloat startY, GLfloat scale, const float colorArray[][3], int numColors, int startColor = 0);
		void renderBg();
		void drawBoard(const tBoard& board, int SIZE);
		void drawContainer(int SIZE);
		void drawFicha(const tFicha& ficha, int size);
		void drawCasilla(RGB color, int i, int j, int SIZE, bool ghost = false);
		~tRenderer();

	private:
		tTexture bitMapFontTexture;
		tTexture bgTexture;
		tFontData fontData;
		unsigned int indices[6] = { 0, 1, 3, 3, 1, 2};
		GLuint VBO;
		GLuint VAO;
		GLuint EBO;	
		void getFontData(string fileName);
		void createBuffers();
		void createTextures(const GLchar* fileName);
		shaderProgram *program;
		void renderChar(const tChar& c, GLfloat startX, GLfloat startY, GLfloat scale, GLfloat r, GLfloat g, GLfloat b);
		

	
};