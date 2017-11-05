#pragma once
#include <iostream>

#include "../glm/glm.hpp"

struct Camera {
	Camera() {}

	Camera(const glm::vec3& p, const int& f, const int& fl, const float& ar)
		: m_position(p)
		, m_fov(f)
		, m_focal_length(fl)
		, m_aspect_ratio(ar)
	{}

	~Camera() {}
private:
	glm::vec3 m_position;
	int m_fov;
	int m_focal_length;
	float m_aspect_ratio;
};

struct Light {
	Light(const glm::vec3& p, const glm::vec3& c)
		: m_position(p)
		, m_color(c)
	{}

	~Light() {}
private:
	glm::vec3 m_position;
	glm::vec3 m_color;
};

struct Sphere {
	Sphere(const glm::vec3& p, const int& r, glm::vec3& ac, const glm::vec3& dc, const glm::vec3& sc, const float& ss)
		: m_position(p)
		, m_radius(r)
		, m_ambient_color(ac)
		, m_diffuse_color(dc)
		, m_specular_color(sc)
		, m_specular_shininess(ss)
	{}

	~Sphere() {}

private:
	glm::vec3 m_position;
	int m_radius;
	glm::vec3 m_ambient_color;
	glm::vec3 m_diffuse_color;
	glm::vec3 m_specular_color;
	float m_specular_shininess;
};

struct Model {
	Model(const std::string& n, const glm::vec3& ac, const glm::vec3& dc, const glm::vec3& sc, const float& ss)
		: m_name(n)
		, m_ambient_color(ac)
		, m_diffuse_color(dc)
		, m_specular_color(sc)
		, m_specular_shininess(ss)
	{}

	~Model() {}

private:
	std::string m_name;
	glm::vec3 m_ambient_color;
	glm::vec3 m_diffuse_color;
	glm::vec3 m_specular_color;
	float m_specular_shininess;
};