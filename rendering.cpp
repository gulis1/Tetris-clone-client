#include "rendering.h"


using namespace std;

void tRenderer::getFontData(string fileName) {

	ifstream file;

	file.open(fileName);
	int id;

	if (file.is_open())

		for (int i = 32; i <= 122; i++) {
			tChar character;

			file >> id;
			file >> character.x;
			file >> character.y;
			file >> character.width;
			file >> character.height;
			file >> character.xoffset;
			file >> character.yoffset;
			file >> character.xadvance;


			file.ignore(1);

			fontData[id] = character;

		}

	else
		cout << "ERROR LOADING FONT DATA" << endl;

}

void tRenderer::createBuffers() {

	glCreateVertexArrays(1, &VAO);
	glCreateBuffers(1, &VBO);
	glCreateBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void tRenderer::createTextures(const GLchar* fileName) {

	stbi_set_flip_vertically_on_load(true);

	glGenTextures(1, &bitMapFontTexture.id);
	glBindTexture(GL_TEXTURE_2D, bitMapFontTexture.id);

	int  nrChannels;
	unsigned char* data = stbi_load(fileName, &bitMapFontTexture.width, &bitMapFontTexture.height, &nrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bitMapFontTexture.width, bitMapFontTexture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


	stbi_image_free(data);

	glGenTextures(1, &bgTexture.id);
	glBindTexture(GL_TEXTURE_2D, bgTexture.id);

	data = stbi_load("bg.png", &bgTexture.width, &bgTexture.height, &nrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bgTexture.width, bgTexture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	stbi_image_free(data);

}

tRenderer::tRenderer() {

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	getFontData("fontData.txt");

	createTextures("font.png");

	program = new shaderProgram("vertex.glsl", "fragment.glsl");
	createBuffers();
}

void tRenderer::renderChar(const tChar& c, GLfloat startX, GLfloat startY, GLfloat scale, GLfloat r, GLfloat g, GLfloat b) {


	GLfloat x, y, w, h;


	x = c.x / GLfloat(bitMapFontTexture.width);
	y = 1 - c.y / GLfloat(bitMapFontTexture.height);

	w = c.width / GLfloat(bitMapFontTexture.width);
	h = c.height / GLfloat(bitMapFontTexture.height);

	GLfloat w2 = w * scale;
	GLfloat h2 = h * scale;


	GLfloat vertices[] = {
		// positions						 // colors				// texture coords
		 startX + w2,  startY + h2, 0.0f,   r, g, b,	     x + w, y,   // top right
		 startX + w2, startY, 0.0f,			r, g, b,		 x + w, y - h,   // bottom right
		 startX, startY, 0.0f,				r, g, b,				 x, y - h,  // bottom left
		 startX,  startY + h2, 0.0f,		r, g, b,		 x, y    // top left 
	};


	GLuint indices[] = {  // note that we start from 0!
	0, 1, 3,   // first triangle
	3, 1, 2
	};


	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


}

void tRenderer::renderText(const string text, GLfloat startX, GLfloat startY, GLfloat scale, const float colorArray[][3], int numColors, int startColor) {

	GLfloat initialX = startX;

	tChar c;


	int cont = startColor;

	glBindTexture(GL_TEXTURE_2D, bitMapFontTexture.id);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glUseProgram(program->id);


	for (const GLchar x : text) {


		if (x == '\n') {
			startY -= 120.0f / float(bitMapFontTexture.height) * scale;
			startX = initialX;
		}


		else {
			int id = int(x);
			tFontData::iterator it;
			it = fontData.find(id);

			if (it != fontData.end()) {
				c = it->second;
				renderChar(c, startX, startY, scale, colorArray[cont][0], colorArray[cont][1], colorArray[cont][2]);
				startX += c.xadvance / float(bitMapFontTexture.width) * scale;

				cont++;
				if (cont >= numColors)
					cont = startColor;
			}
		}

	}

	glUseProgram(0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void tRenderer::renderBg() {

	glBindTexture(GL_TEXTURE_2D, bgTexture.id);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glUseProgram(program->id);


	GLfloat vertices[] = {
		// positions						 // colors				// texture coords
		  1.0,  1.0f, 1.0f,		1.0f, 1.0f, 1.0f,	    0.75f, 1.0f,  // top right
		  1.0f, -1.0f,0.0f,		1.0f, 1.0f, 1.0f,		0.75f, 0.0f,   // bottom right
		 -1.0f, -1.0f,0.0f,		1.0f, 1.0f, 1.0f,		0.25f, 0.0f,	   // bottom left
		 -1.0f,  1.0f,0.0f,		1.0f, 1.0f, 1.0f,		0.25f, 1.0f   // top left 
	};


	GLuint indices[] = {  // note that we start from 0!
	0, 1, 3,   // first triangle
	3, 1, 2
	};

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glUseProgram(0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void tRenderer::drawCasilla(RGB color, int i, int j, int SIZE, bool ghost) {


	glPointSize(SIZE - 2);

	if (!ghost)
		glColor3d(color.r, color.g, color.b);

	else
		glColor4d(color.r / 3.0f, color.g / 3.0f, color.b / 3.0f, 0.7f);

	float offset = SIZE / 2.0;

	GLfloat x = 7 * SIZE + offset + j * SIZE;
	GLfloat y = offset + 30 + i * SIZE + SIZE / 2;

	GLfloat pos[] = { x, y };

	glVertexPointer(2, GL_FLOAT, 0, pos);
	glDrawArrays(GL_POINTS, 0, 1);

}


void tRenderer::drawContainer(int SIZE) {

	glColor4f(0.2, 0.2, 0.2, 0.5f);
	int x1 = 7 * SIZE;
	int x2 = x1 + 10 * SIZE;

	int y1 = 30 + SIZE / 2;
	int y2 = y1 + 20 * SIZE;

	glBegin(GL_POLYGON);
	glVertex2d(x1, y1);
	glVertex2d(x2, y1);
	glVertex2d(x2, y2);
	glVertex2d(x1, y2);
	glEnd();



	glEnd();
}

void tRenderer::drawBoard(const tBoard &board, int SIZE) {

	for (int i = 0; i < 20; i++)
		for (int j = 0; j < 10; j++)
			if (board.board[i][j].state == Occupied)
				drawCasilla(board.board[i][j].color, i, j, SIZE);

}

void tRenderer::drawFicha(const tFicha &ficha, int size) {

	for (auto x : ficha.pos) {
		drawCasilla(ficha.color, x.first, x.second, size);
	}
}



tRenderer::~tRenderer() {
	delete program;
}