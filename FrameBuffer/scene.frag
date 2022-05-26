#version 330 core

in vec2 TextCoord;
uniform sampler2D text;


out vec4 color;


void main()
{
	color = texture(text, vec2(TextCoord.x,1.0-TextCoord.y));
}