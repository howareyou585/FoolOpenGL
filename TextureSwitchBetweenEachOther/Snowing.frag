#version 330

out vec4 color;
uniform sampler2D s_tex1;
uniform sampler2D s_tex2;
uniform float time;
in vec2 texcoord;
void main()
{
	
	color = mix(texture(s_tex1,vec2(texcoord.x, 1-texcoord.y)),texture(s_tex2,vec2(texcoord.x,1-texcoord.y)),time);
}