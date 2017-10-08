#include <vector>
#include <stdio.h>
#include <cstring>
#include <iostream>

#include "..\glew\glew.h" // include GL Extension Wrangler

#include "HeightMapManager.hpp"

const int SKIPMIN = 2;
const int SKIPMAX = 100;
const int SKIPDEFAULT = 20;

const float STEPMIN = 0.1f;
const float STEPMAX = 0.8f;
const float STEPDEFAULT = 0.1f;

/*
	Source - load with Cimg				http://cimg.eu/reference/group__cimg__tutorial.html
	Source - read the color with CImg	https://stackoverflow.com/questions/3291923/how-to-get-rgb-value-by-cimg
*/

HeightMapManager::HeightMapManager():
	m_skipSize(SKIPDEFAULT),
	m_stepSize(STEPDEFAULT)
{

}

HeightMapManager::~HeightMapManager()
{
	~m_image;
}

void HeightMapManager::readSkipSize()
{
	int skipSize;
	while (true)
	{
		std::cout << "Please enter a skip-size integer (" << SKIPMIN << " <= skip-size <= " << SKIPMAX << "): ";
		scanf_s("%d", &skipSize);
		if (skipSize < SKIPMIN || skipSize > SKIPMAX)
			continue;
		else
			break;
	}
	setSkipSize(skipSize);
}

void HeightMapManager::readStepSize()
{
	float stepSize;
	while (true)
	{
		std::cout << "Please enter a skip-size integer (" << STEPMIN << " <= step-size <= " << STEPMAX << "): ";
		scanf_s("%f", &stepSize);
		if (stepSize < STEPMIN || stepSize > STEPMAX)
			continue;
		else
			break;
	}
	setStepSize(stepSize);
}

void HeightMapManager::readImage(std::string& fileName)
{
	m_image = cimg_library::CImg<unsigned char>(fileName.c_str());
	m_image_width = m_image.width();
	m_image_height = m_image.height();
	std::cout << "Image size is " << m_image_width << " x " << m_image_height << std::endl;
}

void HeightMapManager::createAllpoints()
{
	static bool isGrayScale = true;
	for (int r = 0; r < m_image_height; r++)
	{
		for (int c = 0; c < m_image_width; c++)
		{
			// Point Cloud
			glm::vec3 currentColor = glm::vec3((int)m_image(c, r, 0, 0), (int)m_image(c, r, 0, 1), (int)m_image(c, r, 0, 2));
			//cout << "(" << r << "," << c << ") =" << " R" << (int)src(c, r, 0, 0) << " G" << (int)src(c, r, 0, 1) << " B" << (int)src(c, r, 0, 2) << endl;
			if (isGrayScale)
				m_allPoints.push_back(glm::vec3(r, currentColor.x, c));
			else
				m_allPoints.push_back(glm::vec3(r, glm::length(currentColor), c));

			// Subset of points
			if (c % m_skipSize == 0)
				m_subPoints.push_back(m_allPoints.back());
		}
	}
}

void HeightMapManager::loadData()
{
	glGenVertexArrays(1, &VAO);			// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glGenBuffers(1, &VBO);				//

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, m_allPoints.size() * sizeof(glm::vec3), &m_allPoints.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
}

void HeightMapManager::render()
{
	glBindVertexArray(VAO);
	glDrawArrays(GL_POINTS, 0, m_allPoints.size());
	glBindVertexArray(0);
}
