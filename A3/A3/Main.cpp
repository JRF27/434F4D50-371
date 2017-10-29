#include <iostream>

#include "CImg.h"

#include "Parser.hpp"

const int WIDTH = 400;
const int HEIGHT = 400;

const std::string TXT = ".txt";
const std::string BMP = ".bmp";

int main()
{
	std::string file = "sceneOBJ";
	std::string fileName = file.append(TXT);
	Parser::parseSceneText(fileName.c_str());
	std::cout << "Ray tracin'" << std::endl;

	// Creates an image with three channels and sets it to black
	cimg_library::CImg<float> image(WIDTH, HEIGHT, 1, 3, 0);

	// Ray trace...
	// Pixel values must be in the range[0, 255]

	//Save the image in BMP format.
	image.save(file.append(BMP).c_str());

	//Display the rendered image on screen
	cimg_library::CImgDisplay main_disp(image, file.append(BMP).c_str());
	while (!main_disp.is_closed())
		main_disp.wait();

	return 0;
}