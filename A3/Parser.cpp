#include "Parser.hpp"

#include <iostream>
#pragma warning (disable : 4996)
#include <stdio.h>
#include <cstring>

#include <iostream>
#include <fstream>
#include <string>

bool Parser::parseSceneText(int& num, const char * fileLocation, Scene& scene)
{
	std::string line;
	std::ifstream file(fileLocation);

	if (getline(file, line))
	{
		sscanf_s(line.c_str(), "%d", &num);
	}

	while (getline(file, line))
	{
		if (line == "camera")
		{
			glm::vec3 p;
			int f, fl;
			float ar;

			getline(file, line);
			sscanf_s(line.c_str(), "pos: %f %f %f", &p.x, &p.y, &p.z);

			getline(file, line);
			sscanf_s(line.c_str(), "fov: %d", &f);

			getline(file, line);
			sscanf_s(line.c_str(), "f: %d", &fl);

			getline(file, line);
			sscanf_s(line.c_str(), "a: %f", &ar);

			scene.setCamera(Camera(p, f, fl, ar));
		}
		else if (line == "light")
		{
			glm::vec3 p, c;

			getline(file, line);
			sscanf_s(line.c_str(), "pos: %f %f %f", &p.x, &p.y, &p.z);

			getline(file, line);
			sscanf_s(line.c_str(), "col: %f %f %f", &c.x, &c.y, &c.z);

			scene.addLight(Light(p, c));
		}
		else if (line == "model")
		{
			std::string n;
			glm::vec3 ac, dc, sc;
			float ss;

			getline(file, line);
			n = line.c_str();

			getline(file, line);
			sscanf_s(line.c_str(), "amb: %f %f %f", &ac.x, &ac.y, &ac.z);

			getline(file, line);
			sscanf_s(line.c_str(), "dif: %f %f %f", &dc.x, &dc.y, &dc.z);

			getline(file, line);
			sscanf_s(line.c_str(), "spe: %f %f %f", &sc.x, &sc.y, &sc.z);

			getline(file, line);
			sscanf_s(line.c_str(), "shi: %f", &ss);

			scene.addModel(Model(n, ac, dc, sc, ss));
		}
		else if (line == "sphere")
		{
			glm::vec3 p, ac, dc, sc;
			int r;
			float ss;

			getline(file, line);
			sscanf_s(line.c_str(), "pos: %f %f %f", &p.x, &p.y, &p.z);

			getline(file, line);
			sscanf_s(line.c_str(), "rad: %d", &r);

			getline(file, line);
			sscanf_s(line.c_str(), "amb: %f %f %f", &ac.x, &ac.y, &ac.z);

			getline(file, line);
			sscanf_s(line.c_str(), "dif: %f %f %f", &dc.x, &dc.y, &dc.z);

			getline(file, line);
			sscanf_s(line.c_str(), "spe: %f %f %f", &sc.x, &sc.y, &sc.z);

			getline(file, line);
			sscanf_s(line.c_str(), "shi: %f", &ss);

			scene.addSphere(Sphere(p,  r,  ac, dc, sc, ss));
		}
		else if (line == "plane")
		{
			glm::vec3 n, p, ac, dc, sc;
			float ss;

			getline(file, line);
			sscanf_s(line.c_str(), "nor: %f %f %f", &n.x, &n.y, &n.z);

			getline(file, line);
			sscanf_s(line.c_str(), "pos: %f %f %f", &p.x, &p.y, &p.z);

			getline(file, line);
			sscanf_s(line.c_str(), "amb: %f %f %f", &ac.x, &ac.y, &ac.z);

			getline(file, line);
			sscanf_s(line.c_str(), "dif: %f %f %f", &dc.x, &dc.y, &dc.z);

			getline(file, line);
			sscanf_s(line.c_str(), "spe: %f %f %f", &sc.x, &sc.y, &sc.z);

			getline(file, line);
			sscanf_s(line.c_str(), "shi: %f", &ss);

			scene.addPlane(Plane(n, p, ac, dc, sc, ss));
		}
	}

	return true;
}