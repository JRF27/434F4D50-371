#include "stdafx.h"

#include "..\glew\glew.h"					// include GL Extension Wrangler
#include "..\glfw\glfw3.h"					// include GLFW helper library
#include <stdio.h>
#include <algorithm>						// std::random_shuffle
#include <iostream>
#include <string>
#include <fstream>
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

#include "objloader.hpp"					// include the object loader

using namespace std;

// Constants
const GLuint WIDTH = 800;
const GLuint HEIGHT = 800;

const int GRID_SIZE = 20;	// # Lines = GRID_SIZE + 1
const int AXIS_LENGTH = 4;
const int SPHERE_NUMBER = 8;
const GLfloat SCALE_FACTOR = 0.05f;
const glm::vec3 ORIGIN(0.0f, 0.0f, 0.0f);
const glm::vec3 UP(0.0f, 1.0f, 0.0f);

// Enums
enum PacmanDirection { left, right, up, down };

// Globals
glm::vec3 camera_position;
glm::vec3 camera_direction;
glm::vec3 triangle_scale;

PacmanDirection currentDirection = up;
glm::mat4 pacmanLocalRotationMatrix;

// Recipe: projection_matrix * view_matrix * model_matrix
glm::mat4 projection_matrix;
glm::mat4 view_matrix;

// Transforms
glm::mat4 pacmanWorldTransform;
glm::mat4 axisTransform;

// Rotations
glm::mat4 worldRotation;

bool zoomEnabled = false;
bool cameraPanningX = false;
bool cameraTiltingY = false;
GLFWwindow* window;

/**
	This is called whenever a key is pressed/released via GLFW.

	Read: http://www.glfw.org/docs/latest/input_guide.html
	Read: https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glPolygonMode.xhtml
*/
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	static bool toggleMouse = false;
	int temp_x;
	int temp_z;

	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		switch (key)
		{
		case GLFW_KEY_SPACE:
			std::cout << "Re-position player at random." << std::endl;
			temp_x = (rand() % GRID_SIZE) - GRID_SIZE / 2;
			temp_z = (rand() % GRID_SIZE) - GRID_SIZE / 2;
			pacmanWorldTransform = glm::translate(glm::mat4(4), glm::vec3(temp_x, 0.0f, temp_z));
			break;
		case GLFW_KEY_U:
			std::cout << "Scale-up." << std::endl;
			triangle_scale += triangle_scale * SCALE_FACTOR;
			break;
		case GLFW_KEY_J:
			std::cout << "Scale-down." << std::endl;
			triangle_scale -= triangle_scale * SCALE_FACTOR;
			break;
		case GLFW_KEY_A:
			std::cout << "Move Left." << std::endl;
			pacmanWorldTransform = glm::translate(pacmanWorldTransform, glm::vec3(0.0f, 0.0f, -1.0f));
			if (currentDirection != PacmanDirection::left)
			{
				currentDirection = PacmanDirection::left;
				pacmanLocalRotationMatrix = glm::rotate(glm::mat4(1.0f), 1.571f, glm::vec3(0.0f, 1.0f, 0.0f));
			}
			break;
		case GLFW_KEY_D:
			std::cout << "Move Right." << std::endl;
			pacmanWorldTransform = glm::translate(pacmanWorldTransform, glm::vec3(0.0f, 0.0f, 1.0f));
			if (currentDirection != PacmanDirection::right)
			{
				currentDirection = PacmanDirection::right;
				pacmanLocalRotationMatrix = glm::rotate(glm::mat4(1.0f), -1.571f, glm::vec3(0.0f, 1.0f, 0.0f));
			}
			break;
		case GLFW_KEY_W:
			std::cout << "Move Up." << std::endl;
			pacmanWorldTransform = glm::translate(pacmanWorldTransform, glm::vec3(1.0f, 0.0f, 0.0f));
			if (currentDirection != PacmanDirection::up)
			{
				currentDirection = PacmanDirection::up;
				pacmanLocalRotationMatrix = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
			}
			break;
		case GLFW_KEY_S:
			std::cout << "Move Down." << std::endl;
			pacmanWorldTransform = glm::translate(pacmanWorldTransform, glm::vec3(-1.0f, 0.0f, 0.0f));
			if (currentDirection != PacmanDirection::down)
			{
				currentDirection = PacmanDirection::down;
				pacmanLocalRotationMatrix = glm::rotate(glm::mat4(1.0f), 2*1.571f, glm::vec3(0.0f, 1.0f, 0.0f));
			}
			break;
		case GLFW_KEY_LEFT:
			std::cout << "Rotate world -x." << std::endl;
			worldRotation *= glm::rotate(glm::mat4(1.0f), SCALE_FACTOR, glm::vec3(1, 0, 0));
			break;
		case GLFW_KEY_RIGHT:
			std::cout << "Rotate world -x." << std::endl;
			worldRotation *= glm::rotate(glm::mat4(1.0f), -SCALE_FACTOR, glm::vec3(1, 0, 0));
			break;
		case GLFW_KEY_UP:
			std::cout << "Rotate world y." << std::endl;
			worldRotation *= glm::rotate(glm::mat4(1.0f), SCALE_FACTOR, glm::vec3(0, 1, 0));
			break;
		case GLFW_KEY_DOWN:
			std::cout << "Rotate world -y." << std::endl;
			worldRotation *= glm::rotate(glm::mat4(1.0f), -SCALE_FACTOR, glm::vec3(0, 1, 0));
			break;
		case GLFW_KEY_HOME:
			std::cout << "Reset world rotation." << std::endl;
			worldRotation = glm::mat4(1.0f);
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
			std::cout << "Unimplemented key #" << key << "pressed." << std::endl;
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
	projection_matrix = glm::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.0f, 100.0f);
}

/**
	This is called whenever there is an error via GLFW.

	Source: Lecture 03
*/
void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}

/*
	This is called whenever the mouse has moved its position.

	Source: https://learnopengl.com/#!Getting-started/Camera
*/
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{

}

/*

*/
void mouse_button_callback(GLFWwindow *_window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		std::cout << "Left Mouse pressed." << std::endl;
		zoomEnabled = true;
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		std::cout << "Left Mouse released." << std::endl;
		zoomEnabled = false;
	}
	else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
	{
		std::cout << "Middle Mouse pressed." << std::endl;
		cameraTiltingY = true;
	}
	else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
	{
		std::cout << "Middle Mouse released." << std::endl;
		cameraTiltingY = false;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		std::cout << "Right Mouse pressed." << std::endl;
		cameraPanningX = true;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
	{
		std::cout << "Right Mouse released." << std::endl;
		cameraPanningX = false;
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

	// Z-Buffer
	/*
		GL_ALWAYS	The depth test always passes.
		GL_NEVER	The depth test never passes.
		GL_LESS		Passes if the fragment's depth value is less than the stored depth value.
		GL_EQUAL	Passes if the fragment's depth value is equal to the stored depth value.
		GL_LEQUAL	Passes if the fragment's depth value is less than or equal to the stored depth value.
		GL_GREATER	Passes if the fragment's depth value is greater than the stored depth value.
		GL_NOTEQUAL	Passes if the fragment's depth value is not equal to the stored depth value.
		GL_GEQUAL	Passes if the fragment's depth value is greater than or equal to the stored depth value.
	*/
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

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

int main()
{
	if (init() == 1)
	{
		std::cout << "Initialization complete!" << std::endl;
	}

	// Define the viewport dimensions and the camera's perspective
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	glViewport(0, 0, width, height);
	projection_matrix = glm::perspective(45.0f, (GLfloat) width / (GLfloat) height, 0.0f, 100.0f);

	/*
		Start of Shaders
	*/

	// Build and compile our shader program
	// Vertex shader

	// Read the Vertex Shader code from the file
	string vertex_shader_path = "vertex.shader";
	string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_shader_path, ios::in);

	if (VertexShaderStream.is_open())
	{
		string Line = "";
		while (getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	else
	{
		printf("Impossible to open %s. Are you in the right directory ?\n", vertex_shader_path.c_str());
		getchar();
		exit(-1);
	}

	// Read the Fragment Shader code from the file
	string fragment_shader_path = "fragment.shader";
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_shader_path, std::ios::in);

	if (FragmentShaderStream.is_open())
	{
		std::string Line = "";
		while (getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}
	else
	{
		printf("Impossible to open %s. Are you in the right directory?\n", fragment_shader_path.c_str());
		getchar();
		exit(-1);
	}

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(vertexShader, 1, &VertexSourcePointer, NULL);
	glCompileShader(vertexShader);
	// Check for compile time errors
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(fragmentShader, 1, &FragmentSourcePointer, NULL);
	glCompileShader(fragmentShader);
	// Check for compile time errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Link shaders
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// Check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader); //free up memory
	glDeleteShader(fragmentShader);

	glUseProgram(shaderProgram);

	/*
		End of Shaders
	*/

	/*
		Start of Object Data
	*/

	// Object 1
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> UVs;
	loadOBJ("pacman.obj", false, vertices, normals, UVs); //read the vertices from the .obj file

	std::vector<glm::vec3> colorsPacman;
	for (int i = 0; i < vertices.size(); i++)
	{
		colorsPacman.push_back(glm::vec3(1.0, 1.0, 0.0));
	}

	GLuint VAO_pacman;
	GLuint verticesPacman_VBO;
	GLuint colorsPacman_VBO;

	glGenVertexArrays(1, &VAO_pacman);		// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glGenBuffers(1, &verticesPacman_VBO);	//
	
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO_pacman);
	glBindBuffer(GL_ARRAY_BUFFER, verticesPacman_VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &colorsPacman_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, colorsPacman_VBO);
	glBufferData(GL_ARRAY_BUFFER, colorsPacman.size() * sizeof(glm::vec3), &colorsPacman.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);	// Unbind VBO
	glBindVertexArray(0);				// Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)

	// Object 2
	GLuint VAO_lines;
	GLuint verticesLines_VBO;
	GLuint colorsLines_VBO;

	std::vector<glm::vec3> points;
	for (int i = 0; i <= GRID_SIZE; i++)
	{
		// Horizontal Lines
		points.push_back(glm::vec3(i, 0, 0));
		points.push_back(glm::vec3(i, 0, GRID_SIZE));

		// Vertical Lines
		points.push_back(glm::vec3(0, 0, i));
		points.push_back(glm::vec3(GRID_SIZE, 0, i));
	}

	std::vector<glm::vec3> colors;
	for (int i = 0; i < points.size(); i++)
	{
		colors.push_back(glm::vec3(1.0, 1.0, 1.0));
	}

	glGenVertexArrays(1, &VAO_lines);		// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glGenBuffers(1, &verticesLines_VBO);	//

	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO_lines);
	glBindBuffer(GL_ARRAY_BUFFER, verticesLines_VBO);
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), &points.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &colorsLines_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, colorsLines_VBO);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), &colors.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);

	// Object 3
	GLuint VAO_axis;
	GLuint verticesAxis_VBO;
	GLuint colorsAxis_VBO;
	std::vector<glm::vec3> axisPts;
	axisPts.push_back(glm::vec3(0, 0, 0));
	axisPts.push_back(glm::vec3(AXIS_LENGTH, 0, 0));
	axisPts.push_back(glm::vec3(0, 0, 0));
	axisPts.push_back(glm::vec3(0, AXIS_LENGTH, 0));
	axisPts.push_back(glm::vec3(0, 0, 0));
	axisPts.push_back(glm::vec3(0, 0, AXIS_LENGTH));

	std::vector<glm::vec3> axisColors;
	axisColors.push_back(glm::vec3(1.0, 0.0, 0.0));
	axisColors.push_back(glm::vec3(1.0, 0.0, 0.0));
	axisColors.push_back(glm::vec3(0.0, 1.0, 0.0));
	axisColors.push_back(glm::vec3(0.0, 1.0, 0.0));
	axisColors.push_back(glm::vec3(0.0, 0.0, 1.0));
	axisColors.push_back(glm::vec3(0.0, 0.0, 1.0));

	glGenVertexArrays(1, &VAO_axis);		// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glGenBuffers(1, &verticesAxis_VBO);	//

	glBindVertexArray(VAO_axis);
	glBindBuffer(GL_ARRAY_BUFFER, verticesAxis_VBO);
	glBufferData(GL_ARRAY_BUFFER, axisPts.size() * sizeof(glm::vec3), &axisPts.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &colorsAxis_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, colorsAxis_VBO);
	glBufferData(GL_ARRAY_BUFFER, axisColors.size() * sizeof(glm::vec3), &axisColors.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);

	// Object 4
	std::vector<glm::vec3> vertices2;
	std::vector<glm::vec3> normals2;
	std::vector<glm::vec2> UVs2;
	loadOBJ("sphere.obj", false, vertices2, normals2, UVs2); //read the vertices from the .obj file

	std::vector<glm::vec3> colorsSphere;
	for (int i = 0; i < vertices2.size(); i++)
	{
		colorsSphere.push_back(glm::vec3(1.0, 0.0, 0.0));
	}

	GLuint VAO_sphere;
	GLuint verticesSphere_VBO;
	GLuint colorsSphere_VBO;

	glGenVertexArrays(1, &VAO_sphere);		// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glGenBuffers(1, &verticesSphere_VBO);	//

	glBindVertexArray(VAO_sphere);
	glBindBuffer(GL_ARRAY_BUFFER, verticesSphere_VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices2.size() * sizeof(glm::vec3), &vertices2.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &colorsSphere_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, colorsSphere_VBO);
	glBufferData(GL_ARRAY_BUFFER, colorsSphere.size() * sizeof(glm::vec3), &colorsSphere.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);	// Unbind VBO
	glBindVertexArray(0);				// Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)

	// Set initial values
	GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection_matrix");
	GLuint viewMatrixLoc = glGetUniformLocation(shaderProgram, "view_matrix");
	GLuint transformLoc = glGetUniformLocation(shaderProgram, "model_matrix");

	camera_position = glm::vec3(0.0f, 10.0f, 0.0f);
	triangle_scale = glm::vec3(1.0f);

	axisTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	pacmanWorldTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

	// Scales
	glm::mat4 pacmanLocalScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.045f, 0.045f, 0.045f));
	glm::mat4 sphereLocalScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));

	glm::mat4 pacmanLocalTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.0f, -0.5f));
	glm::mat4 sphereLocalTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.0f, 0.5f));
	glm::mat4 gridLocalTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-(GLfloat) GRID_SIZE / 2.0f, 0.0f, -(GLfloat) GRID_SIZE / 2.0f));

	glm::mat4 pacmanCorrectionRotationMatrix = glm::rotate(glm::mat4(1.0f), 0.0f ,glm::vec3(1.0f, 0.0f, 0.0f)); //1.571 radians is 90 degrees

	// Temp objects
	glm::mat4 model_matrixLocal;
	glm::mat4 model_matrix;

	// Randomness
	std::vector<glm::vec3> allGridPoints;
	for (int x = 0; x <= GRID_SIZE; x++)
		for (int z = 0; z <= GRID_SIZE; z++)
			allGridPoints.push_back(glm::vec3(x, 0, z));

	std::random_shuffle(allGridPoints.begin(), allGridPoints.end());

	std::vector<glm::vec3> sphereTransforms;
	for (int i = 0; i < SPHERE_NUMBER; i++)
	{
		sphereTransforms.push_back(allGridPoints.at(i));
	}

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		// Clear the colorbuffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		view_matrix = glm::lookAt(camera_position, camera_direction, glm::vec3(1.0f, 0.0f, 0.0f));//UP);
		model_matrix = glm::scale(model_matrix, triangle_scale);

		// Render
		glBindVertexArray(VAO_pacman);
		model_matrixLocal = pacmanLocalTranslateMatrix * pacmanLocalRotationMatrix * pacmanCorrectionRotationMatrix * pacmanLocalScaleMatrix;
		model_matrix = worldRotation * pacmanWorldTransform * glm::scale(model_matrixLocal, triangle_scale);
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model_matrix));
		glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(view_matrix));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix));
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());

		glBindVertexArray(VAO_lines);
		model_matrixLocal = gridLocalTranslateMatrix;
		model_matrix = worldRotation * model_matrixLocal;
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model_matrix));
		glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(view_matrix));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix));
		glDrawArrays(GL_LINES, 0, points.size());

		glBindVertexArray(VAO_axis);
		model_matrix = axisTransform * worldRotation;
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model_matrix));
		glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(view_matrix));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix));
		glDrawArrays(GL_LINES, 0, axisPts.size());

		for (int i = 0; i < sphereTransforms.size(); i++)
		{
			glBindVertexArray(VAO_sphere);
			model_matrixLocal = glm::translate(glm::mat4(1.0f), sphereTransforms.at(i)) * gridLocalTranslateMatrix * sphereLocalTranslateMatrix * sphereLocalScaleMatrix;
			model_matrix = worldRotation * glm::scale(model_matrixLocal, triangle_scale);
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model_matrix));
			glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(view_matrix));
			glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix));
			glDrawArrays(GL_TRIANGLES, 0, vertices2.size());
		}

		glBindVertexArray(0);

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}
