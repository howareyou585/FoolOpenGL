#version 330

out vec4 color;
uniform sampler2D s_texture;
in vec2 texcoord;
void main()
{
	color = texture(s_texture, texcoord);
}