#include <iostream>

#include "CImg.h"

#include "Parser.hpp"
#include "Scene.hpp"

const int WIDTH = 400;
const int HEIGHT = 400;

const std::string TXT = ".txt";
const std::string BMP = ".bmp";

// Prototypes
glm::vec3 rayTrace(glm::vec3);
int floatToInt(const float& f);
void writeColor(const int& x, const int& y, cimg_library::CImg<float>& image, const glm::vec3& colors);

int main()
{
	Scene scene;
	int num;

	std::string file = "sceneOBJ";
	Parser::parseSceneText(num, std::string(file).append(TXT).c_str(), scene);

	// Creates an image with three channels and sets it to black
	cimg_library::CImg<float> image(WIDTH, HEIGHT, 1, 3, 0);

	int c = scene.getNumberOfObjects();


	//Save the image in BMP format.
	image.save(std::string(file).append(BMP).c_str());

	//Display the rendered image on screen
	cimg_library::CImgDisplay main_disp(image, std::string(file).append(BMP).c_str());
	while (!main_disp.is_closed())
		main_disp.wait();

	return 0;
}


glm::vec3 rayTrace()
{
	glm::vec3 color = glm::vec3(1.0f, 0.0f, 0.0f);
	return color;
}

/*
:
color c = trace(point p, vector d, int step)
{
	color local, reflected, transmitted;
	point q;
	normal n;
	if (step > max) return(background_color);
	q = intersect(p, d, status);11.3 Building a Simple Ray Tracer 565
	if (status == light_source) return(light_source_color);
	if (status == no_intersection) return(background_color);
	n = normal(q);
	r = reflect(q, n);
	t = transmit(q, n);
	local = phong(q, n, r);
	reflected = trace(q, r, step+1);
	transmitted = trace(q, t, step+1);
	return(local + reflected + transmitted);
}
*/


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