#version 330 core
out vec4 color;
uniform sampler2D depthMap;
in vec2 texcoord;
void main()
{
	color = texture(depthMap, texcoord);
}