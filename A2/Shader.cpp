#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>

#include "..\glew\glew.h" // include GL Extension Wrangler

#include "Shader.hpp"

Shader::Shader(std::string fileName, const int type)
{
	m_path = fileName;
	m_type = type;

	std::ifstream shaderStream(m_path, std::ios::in);

	if (shaderStream.is_open())
	{
		std::string Line = "";
		while (getline(shaderStream, Line))
			m_code += "\n" + Line;
		shaderStream.close();
	}
	else
	{
		printf("Impossible to open %s. Are you in the right directory ?\n", m_path.c_str());
		getchar();
		exit(-1);
	}
}

Shader::~Shader()
{

}

GLuint Shader::Compile()
{
	GLuint shader = glCreateShader(m_type);
	char const * sourcePointer = m_code.c_str();
	glShaderSource(shader, 1, &sourcePointer, NULL);
	glCompileShader(shader);
	// Check for compile time errors
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	return shader;
}