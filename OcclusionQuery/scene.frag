#version 450 core

out vec4 FragColor;

in vec2 texcoord;
in vec3 normalDir;
in vec3 fragPos;
struct Light
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 spacular;
} ;
uniform float shiness;
uniform sampler2D texture_diffuse1;
uniform vec3 eyePos;
uniform vec3 spacular;
uniform Light light;
uniform  vec3 color;
void main()
{
	FragColor = vec4(color,1.0f);
	//FragColor = vec4(1.0f,0.f,0.f,1.0f);
}