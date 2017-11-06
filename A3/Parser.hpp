#ifndef PARSER_H
#define PARSER_H
#include <vector>

#include "Scene.hpp"

class Parser {
public:
	static bool parseSceneText(int& num, const char * path, Scene& scene);
};
#endif PARSER_H
