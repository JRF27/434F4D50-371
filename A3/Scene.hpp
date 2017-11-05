#ifndef SCENE_H
#define SCENE_H
#include <vector>

#include "Objects.h"

class Scene {
public:
	Scene() {};
	~Scene() {};

	void setCamera(Camera c) { m_camera = c; };
	void setBackgroundColor(const glm::vec3& c) { m_background_color = c; };
	void addSphere(Sphere s) { m_spheres.push_back(s); };
	void addModel(Model m) { m_models.push_back(m); };
	void addLight(Light l) { m_lights.push_back(l); };

private:
	Camera m_camera;

	glm::vec3 m_background_color;

	std::vector<Sphere> m_spheres;
	std::vector<Model> m_models;
	std::vector<Light> m_lights;
};
#endif SCENE_H