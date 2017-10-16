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

	void readImage(std::string& fileName);

	void start();

	void loadData();
	void render();
	void renderEBO();
	void cycleIndex();
	void cycleRenderMode();
	void renderOriginalEBO();

private:
	void setSkipSize(int& skipSize) { m_skipSize = skipSize; };
	void setStepSize(float& stepSize) { m_stepSize = stepSize; };

	void generateOriginalPoints();
	void generateSubPoints();

	void generateAllIndices();

	void executeCatmullRomX();
	void executeCatmullRomZ();

	std::vector<glm::vec3> catmullRom(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
	std::vector<glm::vec3> linearInterpolation(glm::vec3 p0, glm::vec3 p1);

	void readSkipSize();
	void readStepSize();

	int m_skipSize;
	float m_stepSize;

	cimg_library::CImg<unsigned char> m_image;
	int m_image_width;
	int m_image_height;

	int m_catmull_width;
	int m_catmull_height;

	GLuint m_render_type;

	GLuint VAO;
	GLuint VBO;
	GLuint EBO[3];

	int m_EBO_Index;

	std::vector<unsigned int> m_indicesAll;
	std::vector<unsigned int> m_indicesCatX;
	std::vector<unsigned int> m_indicesSub;

	std::vector<glm::vec3> m_catmullromPoints;

	// Second Buffer
	GLuint VAOo;
	GLuint VBOo;
	GLuint EBOo;

	std::vector<unsigned int> m_originalIndices;
	std::vector<glm::vec3> m_originalPoints;
};
#endif