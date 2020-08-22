#pragma once
#include<glew.h>
#include <glfw3.h>
#include <iostream>
#include <chrono>  
#include "ficha.h"
#include "board.h"
#include <rendering.h>
#include<irrKlang.h>

typedef enum tInputType { None, Keyboard, Controller };

const float renderingColors[7][3] = { {1.0, 1.0, 1.0}, {0.86, 0.153, 0.274}, {0.415, 1.0, 0.0}, {0.7, 0.074, 0.729}, {1.0, 0.514, 0.058}, {0.0, 0.98, 0.929}, {1.0, 0.96, 0.0 } };


typedef struct tInputs {
	tInputType bButton = None;
	tInputType aButton = None;
	tInputType upButton = None;
	tInputType leftButton = None;
	tInputType rightButton = None;
	tInputType rButton = None;
};


typedef struct tConButt {
	bool bButton = false;
	bool aButton = false;
	bool upButton = false;
	bool leftButton = false;
	bool rightButton = false;
	bool rButton = false;
	bool downButton = false;
	const float* axes;
};

typedef struct tUserControlls {
	tInputs inputs;
	tConButt buttons;
	float softDrop = 1.0f;
	
};


typedef struct tFlags {
	bool start = false;
	bool alreadySwapped = 0;
	bool groundedFlag;
	bool firstLanded;
	bool hardDropFlag;
	int lockDownResetCount = 0;
	chrono::steady_clock::time_point lastMoveTime;
	chrono::steady_clock::time_point firstMoveTime;
	bool firstMove = false;
};

typedef struct tScoring {
	int level = 1;
	int score = 0;
	int lines = 0;
	float time = 1000.0f;
};

typedef struct tGameWindow {
	GLFWwindow* window;
	int width;
	int height;
	GLfloat blockSize;
};

class tGame
{
	public:
		tGame(GLFWwindow* window, int width, int height, tRenderer* arenderer, irrklang::ISoundEngine* asoundEngine);
		~tGame();
		irrklang::ISoundEngine* soundEngine;
		tUserControlls userInputs;		
		int gameLoop(GLFWwindow* window);
		tScoring score;
		tGameWindow gameWindow;
		tFlags flags;


	private:	
		
		void controllerInput(GLFWgamepadstate &state);
		bool chooseRotation(int prev, int next);
		void drawGhostPiece();
		void drawNextPiece();
		void drawHoldPiece();
		bool rotate(int next, int wallKickInd1, int wallKickInd2);
		bool outOfBounds(pair<int, int> pos);
		void gravity();
		bool moveRight();
		bool moveLeft();
		bool canFall();
		void place();
		void swap();
		bool checkLose();
		void getNextPiece();
		bool rotateLeft();
		bool rotateRight();
		void hardDrop();
		void resetFlags();
		void drawFrame(GLFWwindow* window);
		tBag bag;
		tBoard board;
		tFicha* piece = nullptr;
		tFicha* nextPiece = nullptr;
		tFicha* ownedPiece = nullptr;
		void keyAction(chrono::steady_clock::time_point& controllerTimer, chrono::steady_clock::time_point ahora);
		tRenderer* renderer;
};
