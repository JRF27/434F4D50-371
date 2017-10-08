#ifndef AXIS_H
#define AXIS_H

class Axis {

public:
	Axis(const int axisLength);

	void loadData();
	void render();

private:
	int m_axisLength;
	GLuint VAO;
	GLuint VBO;

	std::vector<glm::vec3> axisPts;
};
#endif AXIS_H
