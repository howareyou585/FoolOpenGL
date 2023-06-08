#version 330 core

out vec4 color;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_height1;
in vec2 texcoord;
void main()
{
	//color = vec4(0.8, 0.8, 0.0, 1.0);
	color = texture(texture_normal1, texcoord);
}