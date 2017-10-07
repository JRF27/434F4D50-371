#ifndef AXIS_H
#define AXIS_H

class Axis {

public:
	Axis(const int axisLength);

	void render();
private:
	GLuint VAO;
	GLuint VBO;

	std::vector<glm::vec3> axisPts;
};
#endif AXIS_H
