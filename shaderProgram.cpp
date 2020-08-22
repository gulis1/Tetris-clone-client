#include "shaderProgram.h"


shaderProgram::shaderProgram(const string vPath, const string fPath) {

	string vCode;
	string fCode;

	ifstream vFile;
	ifstream fFile;

	stringstream vStream, fStream;
	
	// reading phase
	try {
		vFile.open(vPath);
		fFile.open(fPath);
		
		vStream << vFile.rdbuf();
		fStream << fFile.rdbuf();

		vFile.close();
		fFile.close();

		vCode = vStream.str();
		fCode = fStream.str();

	}

	catch (ifstream::failure e) {
		cout << "ERROR READING SHADER FILES" << endl;
	}

	const GLchar* vShaderCode = vCode.c_str();
	const GLchar* fShaderCode = fCode.c_str();

	int success;
	char infoLog[512];


	// compile phase

	GLuint vertexShader, fragmentShader;

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vShaderCode, NULL);
	glCompileShader(vertexShader);

	// Check for compilation errors
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR COMPILING VERTEX SHADER" << endl << infoLog << std::endl;
	};

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
	glCompileShader(fragmentShader);
	
	// Check for compilation errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);	
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR COMPILING FRAGMENT SHADER" << endl << infoLog << std::endl;
	};

	id = glCreateProgram();
	glAttachShader(id, vertexShader);
	glAttachShader(id, fragmentShader);
	glLinkProgram(id);

	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(id, 512, NULL, infoLog);
		std::cout << "ERROR LINKING SHADER PROGRAM" << endl << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}
