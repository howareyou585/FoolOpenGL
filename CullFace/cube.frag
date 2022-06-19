#version 330

out vec4 color;
in  vec2 TextCoord;
uniform sampler2D text;
void main()
{
	color = texture(text, TextCoord);
}