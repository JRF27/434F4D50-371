#include <iostream>
#include <vector>

#include "CImg.h"

#include "Objects.h"
#include "Parser.hpp"
#include "Scene.hpp"

// Image Plane Dimensions
int WIDTH;
int HEIGHT;

// Center of Projection
glm::vec3 COP;
glm::vec3 TOP_LEFT_CORNER;

// Scene Object
Scene scene;

const std::string TXT = ".txt";
const std::string BMP = ".bmp";

// Prototypes
void setupCamera();
glm::vec3 rayTrace(glm::vec3& rayO, glm::vec3& rayD);
int floatToInt(const float& f);
void writeColor(const int& x, const int& y, cimg_library::CImg<float>& image, const glm::vec3& colors);

int main()
{
	// Set the background color to be black
	scene.setBackgroundColor(glm::vec3(0.0f,0.0f,0.0f));

	int num;
	std::string file = "sceneOBJ";
	Parser::parseSceneText(num, std::string(file).append(TXT).c_str(), scene);

	if (num != scene.getNumberOfObjects())
	{
		std::cout << "Incorrectly defined scene file!" << std::endl;
		return -1;
	}

	// Calc image dimensions and COP
	setupCamera();

	// Creates an image with three channels and sets it to black
	cimg_library::CImg<float> image(WIDTH, HEIGHT, 1, 3, 0);

	// Ray Trace
	for (int y = 0; y < HEIGHT; y++)
	{
		for (int x = 0; x < WIDTH; x++)
		{
			glm::vec3 pointOnImage = glm::vec3(TOP_LEFT_CORNER.x + x, TOP_LEFT_CORNER.y - y, TOP_LEFT_CORNER.z);
			glm::vec3 rayO = pointOnImage;

			glm::vec3 rayD = glm::normalize(rayO + COP);

			glm::vec3 color = rayTrace(COP, rayD);

			writeColor(x, y, image, color);
		}
	}

	//Save the image in BMP format.
	image.save(std::string(file).append(BMP).c_str());

	//Display the rendered image on screen
	cimg_library::CImgDisplay main_disp(image, std::string(file).append(BMP).c_str());
	while (!main_disp.is_closed())
		main_disp.wait();

	return 0;
}

void setupCamera()
{
	Camera camera = scene.getCamera();

	int fov = camera.getFOV();
	int focalLength = camera.getFocalLength();
	float ar = camera.getAR();

	glm::vec3 topLeft = glm::vec3(focalLength  *-ar * glm::tan(glm::radians((float) fov) / 2),
									focalLength * glm::tan(glm::radians((float) fov) / 2),
									-focalLength);

	glm::vec3 topRight = glm::vec3(focalLength * ar * glm::tan(glm::radians((float) fov) / 2),
									focalLength * glm::tan(glm::radians((float) fov) / 2),
									-focalLength);

	glm::vec3 bottomLeft = glm::vec3(focalLength * -ar * glm::tan(glm::radians((float) fov) / 2),
									focalLength * -glm::tan(glm::radians((float) fov) / 2),
									-focalLength);

	WIDTH = (topRight.x - topLeft.x);
	HEIGHT = (topLeft.y - bottomLeft.y);

	// Origin of the rays
	COP = camera.getPosition();

	// Start point of tracing
	TOP_LEFT_CORNER = topLeft;

	std::cout << "Image has width " << WIDTH << " and height " << HEIGHT << "." << std::endl;
}

glm::vec3 rayTrace(glm::vec3& rayO, glm::vec3& rayD)
{
	glm::vec3 color(0.0f, 0.0f, 0.0f);

	std::vector<Primitive*> primitives = scene.getPrimitives();
	std::vector<Light*> lights = scene.getLights();

	float distance;						// stores the calculated distance
	float currentDistance = 100000;		// closest distance found
	int closestIndex = -1;				// index of that distance

	// Find closest object
	for (int i = 0; i < primitives.size(); i++)
	{
		bool t = primitives.at(i)->intersect(rayO, rayD, distance);
		if (t && distance <= currentDistance)
		{
			currentDistance = distance;
			closestIndex = i;
		}
	}

	// No Hit
	if (closestIndex == -1)
	{
		color = scene.getBackgroundColor();
	}
	// Hit Something!
	else
	{
		// 1) get the ambient color of the object that was hit
		color += primitives.at(closestIndex)->getAmbientColor();

		// 2) where was the hit in space
		glm::vec3 hitPoint = rayO + rayD * currentDistance;

		// 3) consider the potential affect of each light source
		for (int j = 0; j < lights.size(); j++)
		{
			// Vector from the hit point to the light source
			glm::vec3 backtrackToLight = lights.at(j)->getPosition() - hitPoint;

			// Normalized light direction
			glm::vec3 lightDirection = glm::normalize(backtrackToLight);
			currentDistance = 100000;		// reset

			for (int i = 0; i < primitives.size(); i++)
			{
				bool t = primitives.at(i)->intersect(hitPoint + lightDirection*0.01f, lightDirection, distance);
				if (t && distance <= currentDistance)
				{
					currentDistance = distance;
				}
			}

			// Is the light blocked by an object? if yes, then check another light source
			if (currentDistance < glm::length(backtrackToLight))
			{
				continue;
			}
			else
			{
				// Formula parameters
				float alpha = primitives.at(closestIndex)->getSpecularFactor();
				glm::vec3 Kdif = primitives.at(closestIndex)->getDiffuseColor();
				glm::vec3 Kspe = primitives.at(closestIndex)->getSpecularColor();

				glm::vec3 normalDirection = primitives.at(closestIndex)->getNormal(hitPoint);
				glm::vec3 reflectionDirection = glm::reflect(-lightDirection, normalDirection);

				// Formula
				// light_color * (Kdiff * (L.N) + Kspe * pow((R.V),alpha))
				glm::vec3 light_color = lights.at(j)->getColor();
				glm::vec3 formula = (Kdif * glm::clamp(glm::dot(lightDirection, normalDirection), 0.0f, 1.0f)) + (Kspe * glm::pow(glm::clamp(glm::dot(reflectionDirection, -rayD), 0.0f, 1.0f), alpha));

				// Add the color's components
				color.x += light_color.x * formula.x;
				color.y += light_color.y * formula.y;
				color.z += light_color.z * formula.z;
			}
		}
	}
	
	// Keep the values in between 0.0f and 1.0f
	return glm::clamp(color, 0.0f, 1.0f);
}

int floatToInt(const float& f)
{
	return f * 255;
}

void writeColor(const int& x, const int& y, cimg_library::CImg<float>& image, const glm::vec3& colors)
{
	image(x, y, 0) = floatToInt(colors.x);	//R
	image(x, y, 1) = floatToInt(colors.y);	//G
	image(x, y, 2) = floatToInt(colors.z);	//B
}