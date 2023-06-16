#version 330

out vec4 color;
uniform sampler2D texure_s;
in vec2 texcoord;

void main()
{
	color = texture(texure_s,texcoord);
}