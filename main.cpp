#include<glew.h>
#include <glfw3.h>
#include <iostream>  
#include "game.h"
#include<rendering.h>
#include <chrono>
#include<irrKlang.h>
#include "tSocket.h"

using namespace std;

const string ADDRESS = "someAdress";
const string PORT = "somePort";


void windowResize(GLFWwindow* window, int width, int height);
void onKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods);
void menuKeyhandler(GLFWwindow* window, int key, int scancode, int action, int mods);
void nameInputKeyhandler(GLFWwindow* window, int key, int scancode, int action, int mods);
void character_callback(GLFWwindow* window, unsigned int codepoint);
string nameInput(GLFWwindow* window, tRenderer& renderer);


int main()
{
	WSADATA data;
	WSAStartup(0x202, &data);

	srand(time(NULL));
    
    /* Initialize the library */
    if (!glfwInit())
        return -1;

	GLFWwindow* window;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(480, 540, "PekoTetris", NULL, NULL);

	
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
	glfwSetWindowAspectRatio(window, 24, 27);
	glfwSetWindowSizeLimits(window, 480, 540, GLFW_DONT_CARE, GLFW_DONT_CARE);

	glewInit();
	
	glViewport(0.0f, 0.0f, 480, 540);
	glMatrixMode(GL_PROJECTION);				
	glLoadIdentity();
	glOrtho(0, 480, 0, 540, 0, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	tGame* game;

	int width = 480;
	int height = 540;

	tRenderer *renderer = new tRenderer();

	string user = nameInput(window, *renderer);

	glfwSetWindowSizeCallback(window, windowResize);

	irrklang::ISoundEngine* soundEngine = irrklang::createIrrKlangDevice();
	irrklang::ISound* snd = soundEngine->play2D("ost.mp3", true, false, true);
	snd->setVolume(0.1f);

	tSocket sock(ADDRESS, PORT);
	string callString;
	const char* call;
	string response;
	int score = 0;

	GLFWgamepadstate state;


	while (!glfwWindowShouldClose(window)) {

		snd->setIsPaused(true);
		glfwSetKeyCallback(window, menuKeyhandler);
		
		callString = "post," + user + "," +  to_string(score);
		call = callString.c_str();
		sock.sendMsg(call);

		callString = "get," + user;
		call = callString.c_str();
		response = sock.sendMsg(call);
			   
		game = new tGame(window, width, height, renderer, soundEngine);
		glfwSetWindowUserPointer(window, game);

		while (!glfwWindowShouldClose(window) && !game->flags.start) {
			
			glClear(GL_COLOR_BUFFER_BIT);
			renderer->renderBg();
			renderer->renderText("TETRIS", -0.48f, 0.8f, 2.0f, renderingColors, 7, 1);

			renderer->renderText("Playing as: " + user, -0.5f, -0.9f, 0.7f, renderingColors, 1);

			if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1)) {
				glfwGetGamepadState(GLFW_JOYSTICK_1, &state);
				if (state.buttons[GLFW_GAMEPAD_BUTTON_A] == GLFW_PRESS)
					game->flags.start = true;
				renderer->renderText("Press  A  to start", -0.7f, -0.3f, 1.0f, renderingColors, 1);

			}
			else {
				renderer->renderText("Press Enter to start", -0.85f, -0.4f, 1.0f, renderingColors, 1);
			}

			if (response != "0")
				renderer->renderText(response, -0.7f, 0.5f, 0.8f, renderingColors, 1);

			else
				renderer->renderText("Could not connect to server", -0.7f, 0.6f, 0.6f, renderingColors, 1);

			
			if (score != 0)
				renderer->renderText("Last score: " + to_string(score), -0.6f, -0.7f, 1.0f, renderingColors, 1);

				
			glfwSwapBuffers(window);
			glfwPollEvents();

		}
		
		glfwSetKeyCallback(window, onKeyPress);
		snd->setPlayPosition(0);
		snd->setIsPaused(false);
		score = game->gameLoop(window);
		height = game->gameWindow.height;
		width = game->gameWindow.width;
		delete game;
		soundEngine->play2D("lose.mp3");
		
	}
		
	delete renderer;
	glfwTerminate();
	WSACleanup();

    return 0;
}


void windowResize(GLFWwindow* window, int width, int height)
{
	tGame* game = (tGame*)glfwGetWindowUserPointer(window);

	bool maximized = glfwGetWindowAttrib(window, GLFW_MAXIMIZED);

	if (maximized) {
		glfwRestoreWindow(window);
	}
	else
	{
		game->gameWindow.width = width;
		game->gameWindow.height = height;
		game->gameWindow.blockSize = float(width) / 24.0f;

		glViewport(0.0f, 0.0f, width, height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, width, 0, height, 0, 1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}
}

void onKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods) {

	tGame* game = (tGame*)glfwGetWindowUserPointer(window);


	if (key == GLFW_KEY_P && action == GLFW_PRESS)
		game->userInputs.inputs.bButton = Keyboard;

	if (key == GLFW_KEY_O && action == GLFW_PRESS)
		game->userInputs.inputs.aButton = Keyboard;

	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		game->userInputs.inputs.upButton = Keyboard;

	if (key == GLFW_KEY_L && action == GLFW_PRESS)
		game->userInputs.inputs.rButton = Keyboard;

	if (key == GLFW_KEY_D && action == GLFW_PRESS) {
		game->userInputs.inputs.rightButton = Keyboard;
		game->flags.firstMove = true;
		game->flags.firstMoveTime = chrono::steady_clock::now();
	}

	if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
		game->userInputs.inputs.rightButton = None;
		game->flags.firstMove = false;
	}

	if (key == GLFW_KEY_A && action == GLFW_PRESS) {
		game->userInputs.inputs.leftButton = Keyboard;
		game->flags.firstMove = true;
		game->flags.firstMoveTime = chrono::steady_clock::now();
	}

	if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
		game->userInputs.inputs.leftButton = None;
		game->userInputs.inputs.rightButton = None;
		game->flags.firstMove = false;
	}

	if (key == GLFW_KEY_S && action == GLFW_PRESS)
		game->userInputs.softDrop = 20.0f;

	if (key == GLFW_KEY_S && action == GLFW_RELEASE)
		game->userInputs.softDrop = 1.0f;

}

void menuKeyhandler(GLFWwindow* window, int key, int scancode, int action, int mods) {
	tGame* game = (tGame*)glfwGetWindowUserPointer(window);

	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
		game->flags.start = true;
}

void nameInputKeyhandler(GLFWwindow* window, int key, int scancode, int action, int mods) {
	string* name = (string*)glfwGetWindowUserPointer(window);

	if (key == GLFW_KEY_BACKSPACE && (action == GLFW_PRESS || action == GLFW_REPEAT))
		if (name->size() > 0)
			name->pop_back();

	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
		*name += '\0';
	}

}

void character_callback(GLFWwindow* window, unsigned int codepoint)
{
	string* name = (string*)glfwGetWindowUserPointer(window);
	*name += char(codepoint);
}

string nameInput(GLFWwindow* window, tRenderer& renderer) {
	string name = "";
	glfwSetKeyCallback(window, nameInputKeyhandler);
	glfwSetCharCallback(window, character_callback);
	glfwSetWindowUserPointer(window, &name);

	while (!glfwWindowShouldClose(window)) {

		glClear(GL_COLOR_BUFFER_BIT);
		renderer.renderBg();
		renderer.renderText("TETRIS", -0.48f, 0.8f, 2.0f, renderingColors, 7, 1);

		renderer.renderText("Input your name:", -0.5f, 0.2f, 0.7f, renderingColors, 1, 2);

		renderer.renderText(name, -0.5f, 0.0f, 0.7f, renderingColors, 1);


		if (name.size() > 10)
			renderer.renderText("Name is too long!", -0.5f, -0.3f, 0.7f, renderingColors, 1, 1);

		if (name.find_first_of('\0') != string::npos) {

			if (name.size() == 1) {
				name.pop_back();
			}

			else if (name.size() <= 10) {
				name.pop_back();
				break;
			}
		}
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwSetCharCallback(window, 0);
	return name;
}