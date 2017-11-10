#pragma once
#include <iostream>

#include "../glm/glm.hpp"

static bool QuadtraticFormula(const float& a, const float& b, const float& c, float& sol1, float& sol2);

struct Camera {
	Camera() {}

	Camera(const glm::vec3& p, const int& f, const int& fl, const float& ar)
		: m_position(p)
		, m_fov(f)
		, m_focal_length(fl)
		, m_aspect_ratio(ar)
	{}

	~Camera() {}

	glm::vec3 getPosition() { return m_position; };
	int getFOV() { return m_fov; };
	int getFocalLength() { return m_focal_length; };
	float getAR() { return m_aspect_ratio; };

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

	glm::vec3 getPosition() { return m_position; };
	glm::vec3 getColor() { return m_color; };

	~Light() {}
private:
	glm::vec3 m_position;
	glm::vec3 m_color;
};

class Primitive {
public:
	Primitive(const glm::vec3& p, glm::vec3& ac, const glm::vec3& dc, const glm::vec3& sc, const float& ss)
		: m_position(p)
		, m_ambient_color(ac)
		, m_diffuse_color(dc)
		, m_specular_color(sc)
		, m_specular_shininess(ss)
	{}

	virtual glm::vec3 getNormal(const glm::vec3& point) = 0;
	virtual bool intersect(glm::vec3& origin, glm::vec3& direction, float& distance) = 0;

	glm::vec3 getPosition() { return m_position; };
	glm::vec3 getAmbientColor() { return m_ambient_color; };
	glm::vec3 getDiffuseColor() { return m_diffuse_color; };
	glm::vec3 getSpecularColor() { return m_specular_color; };
	float getSpecularFactor() { return m_specular_shininess; };

private:
	glm::vec3 m_position;
	glm::vec3 m_ambient_color;
	glm::vec3 m_diffuse_color;
	glm::vec3 m_specular_color;
	float m_specular_shininess;
};

class Sphere : public Primitive {
public:
	Sphere(const glm::vec3& p, const int& r, glm::vec3& ac, const glm::vec3& dc, const glm::vec3& sc, const float& ss)
		: Primitive(p, ac, dc, sc, ss)
		, m_radius(r)
	{}

	~Sphere() {}

	glm::vec3 getNormal(const glm::vec3& point);
	bool intersect(glm::vec3& origin, glm::vec3& direction, float& distance);

private:
	int m_radius;
};

class Plane : public Primitive {
public:
	Plane(const glm::vec3& n, const glm::vec3& p, glm::vec3& ac, const glm::vec3& dc, const glm::vec3& sc, const float& ss)
		: Primitive(p, ac, dc, sc, ss)
		, m_normal(n)
	{}

	~Plane() {}

	glm::vec3 getNormal(const glm::vec3& point);
	bool intersect(glm::vec3& origin, glm::vec3& direction, float& distance);

private:
	glm::vec3 m_normal;
};

/*
	Should contain triangles defined by each triangle in the model...
*/
struct Model {
	Model(const std::string& n, const int& c, const glm::vec3& ac, const glm::vec3& dc, const glm::vec3& sc, const float& ss)
		: m_name(n)
		, m_triangle_count(c)
		, m_ambient_color(ac)
		, m_diffuse_color(dc)
		, m_specular_color(sc)
		, m_specular_shininess(ss)
	{}

	~Model() {}

	int getTriangleCount() { return m_triangle_count; };

private:
	std::string m_name;
	int m_triangle_count;
	glm::vec3 m_ambient_color;
	glm::vec3 m_diffuse_color;
	glm::vec3 m_specular_color;
	float m_specular_shininess;
};

class Triangle : public Primitive {
public:
	Triangle(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3, glm::vec3& ac, const glm::vec3& dc, const glm::vec3& sc, const float& ss)
		: Primitive(glm::vec3(0.0f,0.0f,0.0f), ac, dc, sc, ss)	// dummy position
		, m_vertex1(v1)
		, m_vertex2(v2)
		, m_vertex3(v3)
	{}

	~Triangle() {}

	glm::vec3 getNormal(const glm::vec3& point);
	bool intersect(glm::vec3& origin, glm::vec3& direction, float& distance);

private:
	glm::vec3 m_vertex1;
	glm::vec3 m_vertex2;
	glm::vec3 m_vertex3;
};
