#include <iostream>

#include "..\glew\glew.h" // include GL Extension Wrangler

#include "ShaderProgram.hpp"
#include "Shader.hpp"

ShaderProgram::ShaderProgram(std::string vertexPath, std::string fragmentPath)
{
	Shader vertexShader = Shader(vertexPath, GL_VERTEX_SHADER);
	Shader fragmentShader = Shader(fragmentPath, GL_FRAGMENT_SHADER);

	GLuint iV = vertexShader.Compile();
	GLuint iF = fragmentShader.Compile();

	m_id = glCreateProgram();
	glAttachShader(m_id, iV);
	glAttachShader(m_id, iF);
	glLinkProgram(m_id);

	GLint success;
	GLchar infoLog[512];

	// Check for linking errors
	glGetProgramiv(m_id, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(m_id, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(iV); //free up memory
	glDeleteShader(iF);

}

void ShaderProgram::Run()
{
	glUseProgram(m_id);
}