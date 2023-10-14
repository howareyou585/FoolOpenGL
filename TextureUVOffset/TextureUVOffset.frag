#version 430 core

out vec4 FragColor;
in vec2 texCoord;
uniform sampler2D sampler2d;
uniform vec2 uvOffset;
void main()
{
	FragColor = texture(sampler2d, texCoord + uvOffset);
}