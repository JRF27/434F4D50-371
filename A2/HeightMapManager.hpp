#ifndef HEIGHTMAPMANAGER_H
#define HEIGHTMAPMANAGER_H

#include "CImg.h"
#include "glm.hpp"

class HeightMapManager {

public:
	HeightMapManager();
	~HeightMapManager();

	int getSkipSize()						{ return m_skipSize; };
	float getStepSize()						{ return m_stepSize; };
	std::vector<glm::vec3> getAllPoints()	{ return m_allPoints; };

	void readSkipSize();
	void readStepSize();
	void readImage(std::string& fileName);
	void createAllpoints();
	void loadData();
	void render();

	void executeCatmullRom();

private:
	void setSkipSize(int& skipSize) { m_skipSize = skipSize; };
	void setStepSize(float& stepSize) { m_stepSize = stepSize; };

	void catmullRom(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);

	int m_skipSize;
	float m_stepSize;

	cimg_library::CImg<unsigned char> m_image;
	int m_image_width;
	int m_image_height;

	GLuint VAO;
	GLuint VBO;
	GLuint EBO;

	glm::vec3 m_v3_triangleScale;
	std::vector<glm::vec3> m_allPoints;
	std::vector<glm::vec3> m_subPoints;
};

#endif