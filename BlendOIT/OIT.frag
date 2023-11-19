#version 330

out vec4 color;
uniform sampler2D s_texture;
in vec2 texcoord;
void main()
{
	vec4 texColor = texture(s_texture, texcoord);
	color = texColor;
}