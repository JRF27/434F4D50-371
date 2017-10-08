#include "stdafx.h"

#include "..\glew\glew.h"					// include GL Extension Wrangler
#include "..\glfw\glfw3.h"					// include GLFW helper library
#include <stdio.h>
#include <algorithm>						// std::random_shuffle
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

#include "HeightMapManager.hpp"
#include "ShaderProgram.hpp"
#include "Axis.hpp"

using namespace std;

typedef uint32_t uint32;
typedef float float32;
typedef double float64;

// Constants
const GLuint WIDTH = 800;
const GLuint HEIGHT = 800;

const GLfloat SCALE_FACTOR = 0.05f;
const glm::vec3 ORIGIN(0.0f, 0.0f, 0.0f);
const glm::vec3 UP(0.0f, 1.0f, 0.0f);

// Globals
glm::vec3 cameraPosition;
glm::vec3 cameraTarget;
glm::vec3 cameraUp;

// Recipe: projection_matrix * view_matrix * model_matrix
glm::mat4 projection_matrix;
glm::mat4 view_matrix;

// Mouse
glm::vec2 lastMousePoint = glm::vec2(WIDTH / 2, HEIGHT / 2);
bool cameraZoomingEnabled = false;
bool cameraPanningEnabled = false;
bool cameraTiltingEnabled = false;

// Window
GLFWwindow* window;

// Function prototypes
void resetCamera();

/**
	This is called whenever a key is pressed/released via GLFW.

	Read: http://www.glfw.org/docs/latest/input_guide.html
	Read: https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glPolygonMode.xhtml
*/
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	static bool toggleMouse = false;

	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		switch (key)
		{
		case GLFW_KEY_A:
			std::cout << "Move Left." << std::endl;
			cameraPosition += glm::vec3(0.0f,0.0f,-2.0f);
			break;
		case GLFW_KEY_D:
			std::cout << "Move Right." << std::endl;
			cameraPosition += glm::vec3(0.0f, 0.0f, 2.0f);
			break;
		case GLFW_KEY_W:
			std::cout << "Move Forard." << std::endl;
			cameraPosition += glm::vec3(2.0f, 0.0f, 0.0f);
			break;
		case GLFW_KEY_S:
			std::cout << "Move Backward." << std::endl;
			cameraPosition += glm::vec3(-2.0f, 0.0f, 0.0f);
			break;
		case GLFW_KEY_P:
			std::cout << "Rendered as Points." << std::endl;
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
			break;
		case GLFW_KEY_L:
			std::cout << "Rendered as Lines." << std::endl;
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			break;
		case GLFW_KEY_T:
			std::cout << "Rendered as Triangles." << std::endl;
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			break;
		case GLFW_KEY_ESCAPE:
			std::cout << "Escape pressed." << std::endl;
			toggleMouse = !toggleMouse;
			toggleMouse ? glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL) : glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			break;
		default:
			//std::cout << "Unimplemented key #" << key << "pressed." << std::endl;
			break;
		}
	}
}

/**
	This is called whenever the window has been resized via GLFW.

	Source: 
*/
void window_resize_callback(GLFWwindow *_window, int width, int height)
{
	std::cout << "Resized! " << width << "x" << height << "." << std::endl;
	glViewport(0, 0, width, height);
	projection_matrix = glm::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

/**
	This is called whenever there is an error via GLFW.
*/
void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}

/*
	This is called whenever the mouse's cursor position has changed via GLFW.

	Source: https://learnopengl.com/#!Getting-started/Camera
*/
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	glm::vec2 point = glm::vec2(xpos, ypos);
	//std::cout << "Mouse at (" << point.x << "," << point.y << ")" << std::endl;

	// Calculate delta's
	float xoffset = point.x - lastMousePoint.x;
	float yoffset = lastMousePoint.y - point.y;
	lastMousePoint.x = xpos;
	lastMousePoint.y = ypos;

	if (cameraPanningEnabled)
	{
		cameraPosition += glm::vec3(xoffset * SCALE_FACTOR, 0.0f, 0.0f);
		cameraTarget += glm::vec3(xoffset * SCALE_FACTOR, 0.0f, 0.0f);
		//std::cout << "Camera at (" << cameraPosition.x << "," << cameraPosition.y << "," << cameraPosition.z << ")" << std::endl;
		//std::cout << "Target at (" << cameraTarget.x << "," << cameraTarget.y << "," << cameraTarget.z << ")" << std::endl;
	}

	if (cameraZoomingEnabled)
	{
		cameraPosition += cameraPosition *(yoffset * SCALE_FACTOR);
		//std::cout << "Camera at (" << cameraPosition.x << "," << cameraPosition.y << "," << cameraPosition.z << ")" << std::endl;
	}

	if (cameraTiltingEnabled)
	{
		if (yoffset < 0)
		{
			glm::mat4 mat = glm::rotate(glm::mat4(1.0f), 0.05f, glm::vec3(1.0, 0.0, 0.0));
			cameraPosition = glm::vec3(mat * glm::vec4(cameraPosition, 1.0));
		}
		else
		{
			glm::mat4 mat = glm::rotate(glm::mat4(1.0f), -0.05f, glm::vec3(1.0, 0.0, 0.0));
			cameraPosition = glm::vec3(mat * glm::vec4(cameraPosition, 1.0));
		}
		//std::cout << "Camera at (" << cameraPosition.x << "," << cameraPosition.y << "," << cameraPosition.z << ")" << std::endl;
	}
}

/*
	This is called whenever a mouse button has been pressed/released via GLFW.
*/
void mouse_button_callback(GLFWwindow *_window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		std::cout << "Left Mouse pressed." << std::endl;
		cameraZoomingEnabled = true;
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		std::cout << "Left Mouse released." << std::endl;
		cameraZoomingEnabled = false;
	}
	else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
	{
		std::cout << "Middle Mouse pressed." << std::endl;
		cameraTiltingEnabled = true;
	}
	else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
	{
		std::cout << "Middle Mouse released." << std::endl;
		cameraTiltingEnabled = false;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		std::cout << "Right Mouse pressed." << std::endl;
		cameraPanningEnabled = true;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
	{
		std::cout << "Right Mouse released." << std::endl;
		cameraPanningEnabled = false;
	}
}

/**
	Initialization routine.
*/
int init()
{
	std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;

	// Init GLFW
	glfwInit();

	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	window = glfwCreateWindow(WIDTH, HEIGHT, "A1", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Hide the cursor and capture it.
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Register callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetWindowSizeCallback(window, window_resize_callback);
	glfwSetErrorCallback(error_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	// Blocks swapping until monitor has done at least one vertical draw
	glfwSwapInterval(1);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);		// Passes if the fragment's depth value is less than the stored depth value.

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	return 1;
}

void resetCamera()
{
	cameraPosition = glm::vec3(-100.0f, 200.0f, -100.0f);
	cameraTarget = glm::vec3(200, 200.0f, 200);
	cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
}

void defaults()
{
	// Camera Defaults
	resetCamera();
}

int main()
{
	HeightMapManager* heightMapManager = new HeightMapManager();
	std::string fileName = "depth.bmp";
	heightMapManager->readImage(fileName);
	//heightMapManager->readSkipSize();
	//heightMapManager->readStepSize();
	heightMapManager->createAllpoints();

	if (init() == 1)
	{
		std::cout << "Initialization complete!" << std::endl;
	}

	// Define the viewport dimensions and the camera's perspective
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	glViewport(0, 0, width, height);
	projection_matrix = glm::perspective(45.0f, (GLfloat) width / (GLfloat) height, 0.1f, 1000.0f);

	ShaderProgram* shaderProgramHM = new ShaderProgram("vertex.shader", "fragment.shader");
	ShaderProgram* shaderProgramAxis = new ShaderProgram("vertexAxis.shader", "fragment.shader");
	shaderProgramHM->Run();

	// Set Unifroms
	GLuint projectionLoc = glGetUniformLocation(shaderProgramHM->id(), "projection_matrix");
	GLuint viewMatrixLoc = glGetUniformLocation(shaderProgramHM->id(), "view_matrix");
	GLuint transformLoc = glGetUniformLocation(shaderProgramHM->id(), "model_matrix");

	GLuint projectionLocAxis = glGetUniformLocation(shaderProgramAxis->id(), "projection_matrix");
	GLuint viewMatrixLocAxis = glGetUniformLocation(shaderProgramAxis->id(), "view_matrix");
	GLuint transformLocAxis = glGetUniformLocation(shaderProgramAxis->id(), "model_matrix");

	

	// Position the transforms of the objects
	defaults();

	// Temp objects
	glm::mat4 model_matrix;

	// Matrices / Vectors
	glm::mat4 axisTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	glm::vec3 heightMapScale = glm::vec3(0.5f, 0.5f, 0.5f);

	// FPS
	uint32 frames = 0;
	float64 counter = 0;
	float64 delta = 0;
	float64 currentTime = 0;

	// Load Objects
	Axis* axis = new Axis(100);
	axis->loadData();
	heightMapManager->loadData();

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		currentTime = glfwGetTime();

		// Update Logic

		// Render Logic
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		view_matrix = glm::lookAt(cameraPosition, cameraTarget, cameraUp);
		model_matrix = glm::mat4(1.0f);

		shaderProgramAxis->Run();
		model_matrix = axisTransform;
		glUniformMatrix4fv(transformLocAxis, 1, GL_FALSE, glm::value_ptr(model_matrix));
		glUniformMatrix4fv(viewMatrixLocAxis, 1, GL_FALSE, glm::value_ptr(view_matrix));
		glUniformMatrix4fv(projectionLocAxis, 1, GL_FALSE, glm::value_ptr(projection_matrix));
		axis->render();

		shaderProgramHM->Run();
		model_matrix = glm::mat4(1.0f);
		model_matrix = glm::scale(model_matrix, heightMapScale);
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model_matrix));
		glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(view_matrix));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix));
		heightMapManager->render();

		glfwSwapBuffers(window);

		// FPS
		delta = glfwGetTime() - currentTime;
		counter += delta;
		if (counter >= 1) {
			counter = 0;
			std::cout << "FPS: " << frames << std::endl;
			frames = 0;
		}
		else {
			frames++;
		}
	}

	// Destroy objects...

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}
