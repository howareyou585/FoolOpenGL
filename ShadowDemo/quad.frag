#version 330 core
out vec4 color;
uniform sampler2D depthMap;
in vec2 texcoord;
void main()
{
	float depthVal = texture(depthMap, texcoord).r;
	color = vec4(depthVal,depthVal,depthVal,1.0f);
}