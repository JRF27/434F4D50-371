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
	void addPrimitive(Sphere* s) { m_primitives.push_back(s); };
	void addPrimitive(Plane* p) { m_primitives.push_back(p); };
	void addPrimitive(Triangle* t) { m_primitives.push_back(t); };
	void addModel(Model* m) { m_models.push_back(m); };
	void addLight(Light* l) { m_lights.push_back(l); };

	int getNumberOfObjects()
	{
		// triangle contributions from models
		int triangleModelCount = 0;
		for (auto& model : m_models)
		{
			triangleModelCount += model->getTriangleCount();
		}

		return (m_primitives.size() + m_models.size() + m_lights.size() + 1 - triangleModelCount);
	};

	Camera getCamera() { return m_camera; };
	glm::vec3 getBackgroundColor() { return m_background_color; };
	std::vector<Primitive*> getPrimitives() const { return m_primitives; };
	std::vector<Model*>  getModels() const { return m_models; };
	std::vector<Light*>  getLights() const { return m_lights; };

private:
	Camera m_camera;

	glm::vec3 m_background_color;

	std::vector<Primitive*> m_primitives;
	std::vector<Model*> m_models;
	std::vector<Light*> m_lights;
};
#endif SCENE_H