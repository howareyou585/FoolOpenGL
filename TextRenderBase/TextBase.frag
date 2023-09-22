#version 330

out vec4 color;
uniform sampler2D s_texture;
uniform vec3 textColor;
in vec2 texcoord;
void main()
{
	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(s_texture, texcoord).r);
	color = vec4(textColor,1.0)*sampled;
}