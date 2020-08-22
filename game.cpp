#include "game.h"

tGame::tGame(GLFWwindow* window, int width, int height, tRenderer *arenderer, irrklang::ISoundEngine* asoundEngine) {
	

	soundEngine = asoundEngine;
		
	renderer = arenderer;

	gameWindow.window = window;
	gameWindow.height = height;
	gameWindow.width = width;
	gameWindow.blockSize = float(width) / 24.0f;
}

void tGame::gravity() {

	piece->coords.first--;
	for (int i = 0; i < 4; i++) {
		piece->pos[i].first--;
	}

}

bool tGame::outOfBounds(pair<int, int> pos){

	bool inside = true;

	if (pos.first < 0 || pos.first > 21)
		inside = false;

	else if (pos.second < 0 || pos.second > 9)
		inside = false;

	return inside;


}

bool tGame::rotateRight() {
	
	int rotation = piece->rotation + 1;
	if (rotation > 3)
		rotation = 0;

	return chooseRotation(piece->rotation, rotation);
}

bool tGame::chooseRotation(int prev, int next) {
	
	bool rotated;
	int ind1 = 0;

	if (piece->shape == I)
		ind1 = 1;

	else if (piece->shape == O)
		return false;
		
	
	if	(prev == 0 && next == 1) 
		rotated = rotate(next, ind1, 0);
		
	else if (prev == 1 && next == 0) 
		rotated = rotate(next, ind1, 1);

	else if (prev == 1 && next == 2)
		rotated = rotate(next, ind1, 2);

	else if (prev == 2 && next == 1)
		rotated = rotate(next, ind1, 3);

	else if (prev == 2 && next == 3)
		rotated = rotate(next, ind1, 4);

	else if (prev == 3 && next == 2)
		rotated = rotate(next, ind1, 5);

	else if (prev == 3 && next == 0)
		rotated = rotate(next, ind1, 6);

	else if (prev == 0 && next == 3)
		rotated = rotate(next, ind1, 7);
		

	return rotated;
}

bool tGame::rotateLeft() {

	int rotation = piece->rotation - 1;
	if (rotation < 0)
		rotation = 3;

	return chooseRotation(piece->rotation, rotation);
}

bool tGame::rotate(int next, int wallKickInd1, int wallKickInd2) {
	bool can = false;

	pair<int, int> newPos[4];
	pair<int, int> newCenter;
	int wkick = 0;

	int cont;

	
	while (wkick < 6 && !can) {
		newCenter.first = piece->coords.first + wallKick[wallKickInd1][wallKickInd2][wkick][0];
		newCenter.second = piece->coords.second + wallKick[wallKickInd1][wallKickInd2][wkick][1];

		cont = 0;
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
			{
				if (shapeArray[piece->shape][next][i][j] == 1) {
					newPos[cont].first = newCenter.first - i;
					newPos[cont].second = newCenter.second + j;
					cont++;
				}
			}

		can = true;
		for (auto x : newPos)
		{
			if (!outOfBounds(x))
				can = false;

			else if (board.board[x.first][x.second].state == Occupied)
				can = false;
		}

		wkick++;
	}

	if (can) {
		piece->rotation = next;
		piece->coords.first = newCenter.first;
		piece->coords.second = newCenter.second;
		for (int i = 0; i < 4; i++) {
			piece->pos[i].first = newPos[i].first;
			piece->pos[i].second = newPos[i].second;
		}

	}
	
	return can;

}

bool tGame::moveRight() {
	
	for (int i = 0; i < 4; i++) {
		if (piece->pos[i].second == 9)
			return false;
		else if (board.board[piece->pos[i].first][piece->pos[i].second + 1].state == Occupied)
			return false;
	}

	for (int i = 0; i < 4; i++) {
		piece->pos[i].second++;
	}

	piece->coords.second++;

	return true;
}

bool tGame::moveLeft() {

	
	for (int i = 0; i < 4; i++) {
		if (piece->pos[i].second == 0)
			return false;
		else if (board.board[piece->pos[i].first][piece->pos[i].second - 1].state == Occupied)
			return false;
	}

	for (int i = 0; i < 4; i++) {
		piece->pos[i].second--;
	}

	piece->coords.second--;

	return true;
}

bool tGame::canFall() {

	for (auto x : piece->pos) {

		if (x.first == 0)
			return false;

		else if (board.board[x.first - 1][x.second].state == Occupied)
			return false;
	}

	return true;
}

void tGame::place() {

	for (auto x : piece->pos) {
		board.board[x.first][x.second].state = Occupied;
		board.board[x.first][x.second].color = piece->color;
	}

}

void tGame::drawFrame(GLFWwindow* window) {

	string text = "Points: " + to_string(score.score) + "\n\n" + "Level: " + to_string(score.level);
	
	glClear(GL_COLOR_BUFFER_BIT);
	renderer->renderBg();

	glEnableClientState(GL_VERTEX_ARRAY);
	renderer->drawContainer(gameWindow.blockSize);
	renderer->drawBoard(this->board, gameWindow.blockSize);
	drawGhostPiece();
	renderer->drawFicha(*this->piece, gameWindow.blockSize);
	drawNextPiece();
	drawHoldPiece();
	
	glDisableClientState(GL_VERTEX_ARRAY);

	
	renderer->renderText("TETRIS", -0.48f, 0.8f, 2.0f, renderingColors, 7, 1);
	renderer->renderText(text, -0.95f, -0.7f, 0.5f, renderingColors, 1);


	glfwSwapBuffers(window);
	glfwPollEvents();
	
}

void tGame::hardDrop() {

	while (canFall())
		gravity();
}

void tGame::drawGhostPiece() {

	pair<int, int> ghostCoords[4];

	for (int i = 0; i < 4; i++) {
		ghostCoords[i].first = piece->pos[i].first;
		ghostCoords[i].second = piece->pos[i].second;
	}


	int i = 0;
	bool canFall = true;

	while (canFall) {

		for (int i = 0; i < 4; i++) {
			if (board.board[ghostCoords[i].first - 1][ghostCoords[i].second].state == Occupied || ghostCoords[i].first == 0) {
				canFall = false;
			}			
		}

		if (canFall)
			for (int i = 0; i < 4; i++)
				ghostCoords[i].first--;
	}

	


	for (auto x : ghostCoords) {
		renderer->drawCasilla(piece->color, x.first, x.second, gameWindow.blockSize, true);
	}

}

void tGame::drawNextPiece() {

	for (auto x : nextPiece->pos) {
		renderer->drawCasilla(nextPiece->color, x.first, x.second, gameWindow.blockSize);
	}

}

void tGame::swap() {

	tFicha* aux = ownedPiece;
	
	ownedPiece = piece;
	ownedPiece->coords.first = 17;
	ownedPiece->coords.second = -6;
	ownedPiece->rotation = 0;
	ownedPiece->calcCoords();
	
	if (aux == NULL) {
		piece = nullptr;
		getNextPiece();
	}

	else {
		
		piece = aux;
		piece->coords.first = 21;
		piece->coords.second = 3;
		piece->calcCoords();

	}
}

void tGame::getNextPiece() {

	if (nextPiece == NULL) {
		// First time
		nextPiece = new tFicha(bag.pick());
	}

	if (piece != NULL)
		delete piece;

	// Moves the upcoming piece into the playing field
	piece = nextPiece;

	piece->coords.first = 21;
	piece->coords.second = 3;
	piece->calcCoords();

	nextPiece = new tFicha(bag.pick());

}

void tGame::drawHoldPiece() {
	
	if (ownedPiece != NULL)

		for (auto x : ownedPiece->pos) 
			renderer->drawCasilla(ownedPiece->color, x.first, x.second, gameWindow.blockSize);
	
}

bool tGame::checkLose() {

	for (auto x : board.board[20]) {
		if (x.state == Occupied)
			return true;
	}

	return false;

}

void tGame::resetFlags() {
	flags.alreadySwapped = false;
	flags.firstLanded = false;
	flags.groundedFlag = false;
	flags.hardDropFlag = false;
	flags.lockDownResetCount = 0;
}

void tGame::keyAction(chrono::steady_clock::time_point &controllerTimer, chrono::steady_clock::time_point ahora) {
	
	int timeSinceMoved = chrono::duration_cast<chrono::milliseconds>(ahora - controllerTimer).count();
	int sinceFirstMoved = chrono::duration_cast<chrono::milliseconds>(ahora - flags.firstMoveTime).count();
	bool doneSomething = false;

	if (userInputs.inputs.bButton) {
		doneSomething = rotateRight();
		userInputs.inputs.bButton = None;
	} 

	if (userInputs.inputs.aButton) {
		doneSomething = rotateLeft();
		userInputs.inputs.aButton = None;
	}
	
	if (userInputs.inputs.upButton) {
		hardDrop();
		userInputs.inputs.upButton = None;
		flags.hardDropFlag = true;
	}

	if (userInputs.inputs.rButton) {
		if (!flags.alreadySwapped) {
			swap();
			flags.alreadySwapped = true;
			userInputs.inputs.rButton = None;
			doneSomething = true;
		}
	}

	if (userInputs.inputs.rightButton) {
		if (flags.firstMove ||(sinceFirstMoved > 200 && timeSinceMoved > 15)) {
			doneSomething = moveRight();
			flags.firstMove = false;
			controllerTimer = chrono::steady_clock::now();
		}
	}

	if (userInputs.inputs.leftButton) {
		if (flags.firstMove || (sinceFirstMoved > 200 && timeSinceMoved > 15)) {
			doneSomething = moveLeft();
			flags.firstMove = false;
			controllerTimer = chrono::steady_clock::now();
		}
	}

	if (doneSomething) {

		if (flags.firstLanded && flags.lockDownResetCount < 15) 
			flags.lockDownResetCount++;
	
		flags.lastMoveTime = chrono::steady_clock::now();
	}
	
	// Only 15 moves/rotations are allowed after the piece first lands.
	
	else if (flags.lockDownResetCount >= 15) {
		hardDrop();
		flags.hardDropFlag = true;
	}
		
}

void tGame::controllerInput(GLFWgamepadstate &state) {
	int count;
	glfwGetGamepadState(GLFW_JOYSTICK_1, &state);
	userInputs.buttons.axes= glfwGetJoystickAxes(GLFW_JOYSTICK_1, &count);

	
	if (state.buttons[GLFW_GAMEPAD_BUTTON_B] == GLFW_PRESS && !userInputs.buttons.bButton) {
		userInputs.inputs.bButton = Controller;
		userInputs.buttons.bButton = true;
	}

	else if (state.buttons[GLFW_GAMEPAD_BUTTON_B] == GLFW_RELEASE && userInputs.buttons.bButton) {
		userInputs.buttons.bButton = false;
		userInputs.inputs.bButton = None;
	}

	if (state.buttons[GLFW_GAMEPAD_BUTTON_A] == GLFW_PRESS && !userInputs.buttons.aButton) {
		userInputs.inputs.aButton = Controller;
		userInputs.buttons.aButton = true;
	}

	else if (state.buttons[GLFW_GAMEPAD_BUTTON_A] == GLFW_RELEASE && userInputs.buttons.aButton) {
		userInputs.buttons.aButton = false;
		userInputs.inputs.aButton = None;
	}


	if (state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP] == GLFW_PRESS && !userInputs.buttons.upButton) {
		userInputs.inputs.upButton = Controller;
		userInputs.buttons.upButton = true;
	}

	else if (state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP] == GLFW_RELEASE && userInputs.buttons.upButton) {
		userInputs.inputs.upButton = None;
		userInputs.buttons.upButton = false;	
	}

	if (userInputs.buttons.axes[5] > 0.5f && !userInputs.buttons.rButton) {
		userInputs.inputs.rButton = Controller;
		userInputs.buttons.rButton = true;
	}

	else if (state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP] == GLFW_RELEASE && userInputs.buttons.rButton) {
		userInputs.inputs.rButton = None;
		userInputs.buttons.rButton = false;
	}


	if (state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT] == GLFW_PRESS && !userInputs.buttons.rightButton) {
		userInputs.inputs.rightButton = Controller;
		userInputs.buttons.rightButton = true;
		flags.firstMove = true;
		flags.firstMoveTime = chrono::steady_clock::now();
	}

	else if (state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT] == GLFW_RELEASE && userInputs.buttons.rightButton) {
		userInputs.inputs.rightButton = None;
		userInputs.buttons.rightButton = false;
		userInputs.inputs.rightButton = None;
		flags.firstMove = false;
	}

	if (state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT] == GLFW_PRESS && !userInputs.buttons.leftButton) {
		userInputs.inputs.leftButton = Controller;
		userInputs.buttons.leftButton = true;
		flags.firstMove = true;
		flags.firstMoveTime = chrono::steady_clock::now();
	}

	else if (state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT] == GLFW_RELEASE && userInputs.buttons.leftButton) {
		userInputs.inputs.leftButton = None;
		userInputs.buttons.leftButton = false;
		userInputs.inputs.rightButton = None;
		flags.firstMove = false;
	}
	
	if (state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN] == GLFW_PRESS && !userInputs.buttons.downButton) {
		userInputs.softDrop = 20.0f;
		userInputs.buttons.downButton = true;
	}

	else if (state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN] == GLFW_RELEASE && userInputs.buttons.downButton) {
		userInputs.softDrop = 1.0f;
		userInputs.buttons.downButton = false;
	}

}

int tGame::gameLoop(GLFWwindow* window){
	
	chrono::steady_clock::time_point ahora;
	chrono::steady_clock::time_point fallTimer;
	chrono::steady_clock::time_point frameTimer;
	chrono::steady_clock::time_point controllerTimer;

	bool exit;
	pair<int, int> newScore;

	GLFWgamepadstate state;
	glfwGetGamepadState(GLFW_JOYSTICK_1, &state);

	while (!checkLose() && !glfwWindowShouldClose(window)) {

		getNextPiece();
		fallTimer = chrono::steady_clock::now();
		frameTimer = chrono::steady_clock::now();
		flags.lastMoveTime = chrono::steady_clock::now();
		controllerTimer = chrono::steady_clock::now();

		exit = false;
		resetFlags();

		while (!glfwWindowShouldClose(window) && !flags.hardDropFlag && !exit) {

			if (canFall()) {
				flags.groundedFlag = false;
				ahora = chrono::steady_clock::now();
				if (chrono::duration_cast<chrono::milliseconds>(ahora - fallTimer).count() > score.time / userInputs.softDrop) {
					gravity();
					fallTimer = std::chrono::steady_clock::now();
				}
			}

			else if (!flags.groundedFlag) {
				flags.firstLanded = true;
				flags.groundedFlag = true;
				flags.lastMoveTime = chrono::steady_clock::now();
			}


			ahora = std::chrono::steady_clock::now();
			if (chrono::duration_cast<std::chrono::milliseconds>(ahora - frameTimer).count() > 15) {
				
				drawFrame(window);
			}
			
			if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1)) {

				controllerInput(state);
			}
				
			
			ahora = std::chrono::steady_clock::now();
			keyAction(controllerTimer, ahora);
			
		
			ahora = chrono::steady_clock::now();
			exit = flags.groundedFlag && chrono::duration_cast<chrono::milliseconds>(ahora - flags.lastMoveTime).count() > 500;
		}

		place();
		newScore = board.fullLineCheck(soundEngine);
		score.score += newScore.first;
		score.lines += newScore.second;


		if (score.lines >= 10) {
			score.lines = 10 - score.lines;
			score.level++;
			score.time = pow(0.8 - ((score.level - 1.0f) * 0.007f), score.level - 1.0f)*1000.0f;
		}
		
	}

	return score.score;
}

tGame::~tGame() {
	
	if (piece != NULL) {
		delete piece;
		piece = nullptr;
	}

	if (ownedPiece != NULL) {
		delete ownedPiece;
		ownedPiece = nullptr;
	}

	if (nextPiece != NULL) {
		delete nextPiece;
		nextPiece = nullptr;
	}
}