#version 430 core

out vec4 FragColor;
in vec2 texCoord;
uniform sampler2D sampler2d;
void main()
{
	FragColor = texture(sampler2d, texCoord);
}