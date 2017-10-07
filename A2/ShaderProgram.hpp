#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

class ShaderProgram {

public:
	ShaderProgram(std::string vertexPath, std::string fragmentPath);

	void Run();
	unsigned int id() { return m_id; };

private:
	unsigned int m_id;
};
#endif SHADER_PROGRAM_H
