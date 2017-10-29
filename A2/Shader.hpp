#ifndef SHADER_H
#define SHADER_H

class Shader {

public:
	Shader(std::string fileName, const int type);
	~Shader();

	GLuint Compile();

private:
	std::string m_path;
	std::string m_code;
	int m_type;
};
#endif SHADER_H
