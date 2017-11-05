#include "Parser.hpp"

#include <iostream>
#pragma warning (disable : 4996)
#include <stdio.h>
#include <cstring>

bool Parser::parseSceneText(const char * fileLocation, const Scene& scene)
{
	FILE * file = fopen(fileLocation, "r");
	if (file == NULL)
	{
		std::cout << "File does not exist!" << std::endl;
		//getchar();
		return false;
	}

	while (true)
	{

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break;

		std::cout << lineHeader << std::endl;
	}
}