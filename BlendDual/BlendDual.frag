#version 450 core
layout(location = 0, index = 0) out vec4 first_color;
layout(location = 0, index = 1) out vec4 second_color;
//out vec4 color;
uniform sampler2D s_texture;
in vec2 texcoord;
void main()
{
	vec4 color = texture(s_texture, texcoord);
	first_color = color;
	second_color = color;
}