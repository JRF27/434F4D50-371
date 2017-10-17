#include <vector>
#include <stdio.h>
#include <cstring>
#include <iostream>

#include "..\glew\glew.h" // include GL Extension Wrangler

#include "HeightMapManager.hpp"

const int SKIPMIN = 2;
const int SKIPMAX = 100;
const int SKIPDEFAULT = 20;

const float STEPMIN = 0.05f;
const float STEPMAX = 0.8f;
const float STEPDEFAULT = 0.1f;

/*
	Source - load with Cimg				http://cimg.eu/reference/group__cimg__tutorial.html
	Source - read the color with CImg	https://stackoverflow.com/questions/3291923/how-to-get-rgb-value-by-cimg
*/

HeightMapManager::HeightMapManager():
	m_skipSize(SKIPDEFAULT),
	m_stepSize(STEPDEFAULT),
	m_EBO_Index(0),
	m_render_type(GL_POINTS)
{
}

HeightMapManager::~HeightMapManager()
{
	~m_image;
}

void HeightMapManager::readImage(std::string& fileName)
{
	m_image = cimg_library::CImg<unsigned char>(fileName.c_str());
	m_image_width = m_image.width();
	m_image_height = m_image.height();
	std::cout << "Image size is " << m_image_width << " x " << m_image_height << std::endl;
}

void HeightMapManager::start()
{
	//readSkipSize();
	//readStepSize();
	generateOriginalPoints();
	generateSubPoints();
	executeCatmullRomX();
	executeCatmullRomZ();
	generateAllIndices();
}

void HeightMapManager::loadData()
{
	glGenVertexArrays(1, &VAO);			// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glGenBuffers(1, &VBO);				//
	glGenBuffers(3, &EBO[0]);			//

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, m_catmullromPoints.size() * sizeof(glm::vec3), &m_catmullromPoints.front(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indicesAll.size() * sizeof(unsigned int), &m_indicesAll.front(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indicesSub.size() * sizeof(unsigned int), &m_indicesSub.front(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indicesCatX.size() * sizeof(unsigned int), &m_indicesCatX.front(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glGenVertexArrays(1, &VAOo);		// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glGenBuffers(1, &VBOo);				//
	glGenBuffers(1, &EBOo);				//

	glBindVertexArray(VAOo);
	glBindBuffer(GL_ARRAY_BUFFER, VBOo);
	glBufferData(GL_ARRAY_BUFFER, m_originalPoints.size() * sizeof(glm::vec3), &m_originalPoints.front(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_originalIndices.size() * sizeof(unsigned int), &m_originalIndices.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

void HeightMapManager::render()
{
	glBindVertexArray(VAO);
	glDrawArrays(GL_POINTS, 0, m_catmullromPoints.size());
	glBindVertexArray(0);
}

void HeightMapManager::renderEBO()
{
	glBindVertexArray(VAO);
	switch (m_EBO_Index)
	{
		case(0):
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[m_EBO_Index]);
			glDrawElements(m_render_type, m_indicesAll.size(), GL_UNSIGNED_INT, 0);
			break;
		case(1):
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[m_EBO_Index]);
			glDrawElements(m_render_type, m_indicesSub.size(), GL_UNSIGNED_INT, 0);
			break;
		case(2):
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[m_EBO_Index]);
			glDrawElements(m_render_type, m_indicesCatX.size(), GL_UNSIGNED_INT, 0);
			break;
	}
	glBindVertexArray(0);
}
void HeightMapManager::renderOriginalEBO()
{
	glBindVertexArray(VAOo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOo);
	glDrawElements(m_render_type, m_originalIndices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void HeightMapManager::cycleIndex()
{
	m_EBO_Index = ++m_EBO_Index % (sizeof(EBO) / sizeof(EBO[0]));
}

void HeightMapManager::cycleRenderMode()
{
	static bool cycle = false;

	cycle = !cycle;
	if (cycle)
		m_render_type = GL_TRIANGLES;
	else
		m_render_type = GL_POINTS;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Private
////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HeightMapManager::generateOriginalPoints()
{
	static bool isGrayScale = true;

	int sizeX = m_image_height;
	int sizeZ = m_image_width;

	for (int i = 0; i < (sizeX * sizeZ); i++)
	{
		int x = int(i % sizeX);
		int z = int(i / sizeZ);
		int y = isGrayScale ? (int)m_image(x, z, 0, 0) : glm::length(glm::vec3((int)m_image(x, z, 0, 0), (int)m_image(x, z, 0, 1), (int)m_image(x, z, 0, 2)));
		m_originalPoints.push_back(glm::vec3(x, y, z));
		m_originalIndices.push_back(i);
	}
}

void HeightMapManager::generateSubPoints()
{
	int sizeX = m_image_height;
	int sizeZ = m_image_width;

	m_catmull_width = std::floor(m_image_width / m_skipSize);
	m_catmull_height = std::floor(m_image_height / m_skipSize);

	for (int i = 0; i < (sizeX * sizeZ); i++)
	{
		int x = int(i % sizeX);
		int z = int(i / sizeZ);
		if (x % m_skipSize == 0 && z % m_skipSize == 0)
		{
			m_catmullromPoints.push_back(m_originalPoints.at(i));
		}
	}
}

/*
	Call after catmullrom has been performed.
	Treated as points...
*/
void HeightMapManager::generateAllIndices()
{
	int sizeX = m_image_height;
	int sizeZ = m_image_width;
	for (int i = 0; i < m_catmullromPoints.size(); i++)
	{
		m_catmullromPoints.at(i);
		int x = glm::round(m_catmullromPoints.at(i).x);
		int z = glm::round(m_catmullromPoints.at(i).z);

		m_indicesAll.push_back(i);

		if(z % m_skipSize == 0 )
			m_indicesCatX.push_back(i);
		
		if (x % m_skipSize == 0 && z % m_skipSize == 0)
			m_indicesSub.push_back(i);
	}
}

void HeightMapManager::executeCatmullRomX()
{
	std::vector<glm::vec3> newPoints;
	for (int col = 0; col < m_catmull_width; col++)
	{
		int offset = ( m_catmull_height + 1) * col;

		for (int i = 0; i < m_catmull_height; i++)
		{
			// first point
			if (i == 0)
			{
				newPoints.push_back(m_catmullromPoints.at(i + offset));
				std::vector<glm::vec3> lp = linearInterpolation(m_catmullromPoints.at(i + offset), m_catmullromPoints.at(i + 1 + offset));
				newPoints.insert(newPoints.end(), lp.begin(), lp.end());
				newPoints.push_back(m_catmullromPoints.at(i + 1 + offset));
				continue;
			}

			// last point
			else if (i == m_catmull_height - 1)
			{
				newPoints.push_back(m_catmullromPoints.at(i + offset));
				std::vector<glm::vec3> lp = linearInterpolation(m_catmullromPoints.at(i + offset), m_catmullromPoints.at(i + 1 + offset));
				newPoints.insert(newPoints.end(), lp.begin(), lp.end());
				newPoints.push_back(m_catmullromPoints.at(i + 1 + offset));
				break;
			}
			
			// cmr points
			else
			{
				newPoints.push_back(m_catmullromPoints.at(i + offset));
				std::vector<glm::vec3> cmr = catmullRom(m_catmullromPoints.at(i - 1 + offset), m_catmullromPoints.at(i + offset), m_catmullromPoints.at(i + 1 + offset), m_catmullromPoints.at(i + 2 + offset));
				newPoints.insert(newPoints.end(), cmr.begin(), cmr.end());
			}
		}
	}

	m_catmullromPoints = newPoints;
}

void HeightMapManager::executeCatmullRomZ()
{
	std::vector<glm::vec3> newPoints;

	int xMax = (m_catmullromPoints.size() / m_catmull_height);

	for (int j = 0; j < xMax - 1; j++)
	{
		for (int i = 0; i < m_catmull_width - 2; i++)
		{

			// first point
			if (i == 0)
			{
				newPoints.push_back(m_catmullromPoints.at(j + xMax*(i)));
				std::vector<glm::vec3> lp = linearInterpolation(m_catmullromPoints.at(j + xMax*(i)), m_catmullromPoints.at(j + xMax*(i + 1)));
				newPoints.insert(newPoints.end(), lp.begin(), lp.end());
				newPoints.push_back(m_catmullromPoints.at(j + xMax*(i + 1)));
				continue;
			}

			// last point
			else if (i == m_catmull_width - 1)
			{
				newPoints.push_back(m_catmullromPoints.at(j + xMax*(i)));
				std::vector<glm::vec3> lp = linearInterpolation(m_catmullromPoints.at(j + xMax*(i)), m_catmullromPoints.at(j + xMax*(i + 1)));
				newPoints.insert(newPoints.end(), lp.begin(), lp.end());
				newPoints.push_back(m_catmullromPoints.at(j + xMax*(i + 1)));
				break;
			}

			// cmr points
			else
			{
				newPoints.push_back(m_catmullromPoints.at(j + xMax*(i)));
				std::vector<glm::vec3> cmr = catmullRom(m_catmullromPoints.at(j + xMax*(i - 1)), m_catmullromPoints.at(j + xMax*(i)), m_catmullromPoints.at(j + xMax*(i + 1)), m_catmullromPoints.at(j + xMax*(i + 2)));
				newPoints.insert(newPoints.end(), cmr.begin(), cmr.end());
			}
		}
	}

	m_catmullromPoints = newPoints;
}

std::vector<glm::vec3> HeightMapManager::catmullRom(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3)
{
	static float s = 0.5f;

	glm::mat4 basis = glm::mat4(-s, (2 * s), -s, 0,
								2 - s, s - 3, 0, 1,
								s - 2, 3 - (2 * s), s, 0,
								s, -s, 0, 0);

	glm::mat3x4 ctrl = glm::transpose(glm::mat4x3(p0, p1, p2, p3));

	float u = m_stepSize;

	glm::vec4 uv = glm::vec4(glm::pow(u, 3), glm::pow(u, 2), u, 1);

	glm::vec3 pt = uv * basis * ctrl;

	std::vector<glm::vec3> addedPoints;

	float count = 0.0f;
	do {
		count += u;
		glm::vec4 uv = glm::vec4(glm::pow(count, 3), glm::pow(count, 2), count, 1);
		glm::vec3 pt = uv * basis * ctrl;
		addedPoints.push_back(pt);
	} while (count + 0.1f < 1.0f);

	return addedPoints;
}

std::vector<glm::vec3> HeightMapManager::linearInterpolation(glm::vec3 p0, glm::vec3 p1)
{
	float u = m_stepSize;
	
	std::vector<glm::vec3> addedPoints;

	float count = 0.0f;
	do {
		count += u;
		addedPoints.push_back((1 - count)* p0 + count * p1);
	} while (count + 0.1f < 1.0f);

	return addedPoints;
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
