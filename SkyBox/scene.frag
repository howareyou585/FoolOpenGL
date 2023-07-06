#version 330

out vec4 color;
in vec2 TextCoord;
uniform sampler2D txt;
void main()
{
	color = texture(txt, TextCoord);
}