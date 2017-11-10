#include "Objects.h"

float EPSILON = 0.00001f;

/*
* Finds the solutions to the equation  f(x) = a*x^2 + b*x + c
*/
static bool QuadtraticFormula(const float& a, const float& b, const float& c, float& sol1, float& sol2)
{
	float discriminant = b * b - (4 * a * c);

	// Cannot have a negative square root, if so the solution would be imaginary
	if (discriminant < 0)
		return false;
	// Calculate the two solutions
	else
	{
		sol1 = (-b + glm::sqrt(discriminant)) / (2 * a);
		sol2 = (-b - glm::sqrt(discriminant)) / (2 * a);
		return true;
	}
}

// Sphere
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

glm::vec3 Sphere::getNormal(const glm::vec3& point)
{
	return  glm::normalize(point - getPosition());
}
/*
* Source: Edward Angle book page 566
*
* Parametric sphere equation: (point − center).(point − center) − radius^2 = 0.
*
* Ray parametric equation: p(t) = ray_origin + t*ray_direction
*
* Substituting in the equation of the ray: ray_direction.ray_direction * (t^2) + 2(point − center).(ray_direction)*t + (point − center).(point − center) - r^2 = 0
*/
bool Sphere::intersect(glm::vec3& origin, glm::vec3& direction, float& distance)
{
	///*
	// Want the roots of f(t) = a*t^2 + b*t + c
	float t0, t1;

	float a = glm::dot(direction, direction);
	float b = 2 * glm::dot(origin - getPosition(), direction);
	float c = glm::dot(origin - getPosition(), origin - getPosition()) - glm::pow(m_radius, 2);

	bool solutionExists = QuadtraticFormula(a, b, c, t0, t1);

	distance = -1;
	if (!solutionExists)
	{
		return false;
	}
	else
	{
		if (t0 < 0 && t1 < 0)
			return false;
		else if (t0 > 0 && t1 > 0)
			distance = (t0 > t1) ? t1 : t0;		// get closest solution
		else
			distance = t0 > t1 ? t0 : t1;
	}
	return true;
}

// Plane
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

glm::vec3 Plane::getNormal(const glm::vec3& point)
{
	return  glm::normalize(m_normal);
}

/*
* Source: https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-plane-and-ray-disk-intersection
*/
bool Plane::intersect(glm::vec3& origin, glm::vec3& direction, float& distance)
{
	distance = -1;
	float denominator = glm::dot(m_normal, direction);
	if (abs(denominator) > EPSILON)
	{
		glm::vec3 pointOnPlane = getPosition() - origin;
		distance = glm::dot(pointOnPlane, m_normal) / denominator;
		return (distance >= 0);
	}

	return false;
}

// Triangle
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
* The parameter point is not needed, but is in the signature due to polymorphism.
*/
glm::vec3 Triangle::getNormal(const glm::vec3& point)
{
	glm::vec3 edge1 = m_vertex2 - m_vertex1;
	glm::vec3 edge2 = m_vertex3 - m_vertex1;

	return glm::normalize(glm::cross(edge1, edge2));
}

/*
* Source : https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/moller-trumbore-ray-triangle-intersection
*/
bool Triangle::intersect(glm::vec3& origin, glm::vec3& direction, float& distance)
{
	glm::vec3 v1v2 = m_vertex2 - m_vertex1;		// one edge conencted to vertex1
	glm::vec3 v1v3 = m_vertex3 - m_vertex1;		// other edge connected by vertex1

	glm::vec3 pvec = glm::cross(direction, v1v3);

	float det = glm::dot(v1v2, pvec);

	if (glm::abs(det) < EPSILON)
		return false;

	float invDet = 1 / det;

	glm::vec3 tvec = origin - m_vertex1;

	float u = glm::dot(tvec, pvec) * invDet;
	if (u < 0 || u > 1)
		return false;

	glm::vec3 qVec = glm::cross(tvec, v1v2);
	float v = glm::dot(direction, qVec) * invDet;

	if (v < 0 || u + v > 1)
		return false;

	// Calculate distance from the ray origin at which the intersection happens
	distance = glm::dot(v1v3, qVec) * invDet;

	return (distance > 0);
}
