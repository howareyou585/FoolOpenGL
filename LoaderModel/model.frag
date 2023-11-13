#version 330 core
out vec4 color;
in vec2 texcoord;
uniform sampler2D t0;
uniform sampler2D t1;
uniform sampler2D t2;

void main()
{
	color = texture(t0,texcoord);
}