#version 330 core
out vec4 color;
in vec2 texcoord;
in vec3 nor;
void main()
{
	color = vec4(nor,1.0f);
}