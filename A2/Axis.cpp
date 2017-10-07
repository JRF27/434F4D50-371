#include <vector>

#include "..\glew\glew.h" // include GL Extension Wrangler
#include "glm.hpp"

#include "Axis.hpp"

Axis::Axis(const int axisLength)
{
	axisPts.push_back(glm::vec3(0, 0, 0));
	axisPts.push_back(glm::vec3(axisLength, 0, 0));
	axisPts.push_back(glm::vec3(0, 0, 0));
	axisPts.push_back(glm::vec3(0, axisLength, 0));
	axisPts.push_back(glm::vec3(0, 0, 0));
	axisPts.push_back(glm::vec3(0, 0, axisLength));

	glGenVertexArrays(1, &VAO);		// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glGenBuffers(1, &VBO);			//

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, axisPts.size() * sizeof(glm::vec3), &axisPts.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
}

void Axis::render()
{
	glBindVertexArray(VAO);
	glDrawArrays(GL_LINES, 0, axisPts.size());
	glBindVertexArray(0);
}
