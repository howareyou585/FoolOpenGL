#version 330

out vec4 color;
uniform sampler2D s_tex1;
uniform sampler2D s_tex2;
in vec2 texcoord;
void main()
{
	color = mix(texture(s_tex1,texcoord),texture(s_tex2,texcoord),0.5);
}