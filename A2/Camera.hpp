#ifndef CAMERA_H
#define CAMERA_H

#include "..\glew\glew.h" // include GL Extension Wrangler
#include "glm.hpp"

/*
	Flexible camera, based of a tutorial.

	Source: https://learnopengl.com/#!Getting-started/Camera
*/
class Camera {

public:
	Camera();
	Camera(glm::vec3 position);
	~Camera();

	void setHomePosition(glm::vec3 home) { m_home_position = home; };
	void setWorldUp(glm::vec3 up) { m_worldUp = up; };

	void moveRight();
	void moveLeft();
	void moveForward();
	void moveBackward();
	void moveMouse(int xo, int yo);

	glm::mat4 getViewMatrix();
	void reset();

private:
	void update();

	glm::vec3 m_home_position;
	glm::vec3 m_position;
	glm::vec3 m_worldUp;

	glm::vec3 m_front;
	glm::vec3 m_up;
	glm::vec3 m_right;

	float m_pitch;
	float m_yaw;

	float m_rate;
};
#endif CAMERA_H
