#version 330 core

in vec3 col;
out vec4 color;

void main()
{
	color = vec4(col, 1.0f);
	//color = vec4(1.0f, 0.5f, 0.2f, 1.0f);
} 