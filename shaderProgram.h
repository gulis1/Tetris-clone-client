#pragma once
#include<glew.h>
#include <glfw3.h>
#include <iostream>
#include<fstream>
#include<string>
#include<sstream>

using namespace std;

class shaderProgram
{
	public:
		int id;
		shaderProgram(const string vPath, const string fPath);
};

