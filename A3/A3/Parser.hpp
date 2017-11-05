#ifndef PARSER_H
#define PARSER_H
#include <vector>

#include "Scene.hpp"

class Parser {
public:
	static bool parseSceneText(const char * path, const Scene& scene);
};
#endif PARSER_H
