#version 330

out vec4 color;
uniform sampler2D tex;
in vec2 texcoord;
void main()
{
	color = texture(tex, vec2(texcoord.x,1-texcoord.y));
}