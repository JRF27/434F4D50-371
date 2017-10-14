#include <vector>
#include <iostream>

#include "..\glew\glew.h" // include GL Extension Wrangler
#include "gtc/matrix_transform.hpp"
#include "glm.hpp"

#include "Camera.hpp"

Camera::Camera() : Camera(glm::vec3(0.0f))
{
}

Camera::Camera(glm::vec3 position)
{
	m_home_position = position;
	m_position = position;
	m_up = glm::vec3(0.0f, 1.0f, 0.0f);
	m_worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

	m_yaw = 45.0f;
	m_pitch = 0.0f;
	m_rate = 3.0f;

	update();
}

Camera::~Camera()
{
}

void Camera::moveRight()
{
	m_position += m_right * m_rate;
}
void Camera::moveLeft()
{
	m_position -= m_right * m_rate;
}

void Camera::moveForward()
{
	m_position += m_front * m_rate;
}

void Camera::moveBackward()
{
	m_position -= m_front * m_rate;
}

void Camera::moveMouse(int xo, int yo)
{
	m_yaw += xo;
	m_pitch += yo;

	update();
}

glm::mat4 Camera::getViewMatrix()
{
	return glm::lookAt(m_position, m_position + m_front, m_up);
}

void Camera::reset()
{
	m_position = m_home_position;
	m_up = m_worldUp;

	update();
}

void Camera::update()
{
	glm::vec3 front;
	front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	front.y = sin(glm::radians(m_pitch));
	front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

	m_front = glm::normalize(front);
	m_right = glm::normalize(glm::cross(m_front, m_worldUp));
	m_up = glm::normalize(glm::cross(m_right, m_front));
}
