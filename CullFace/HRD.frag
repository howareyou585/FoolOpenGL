#version 330

out vec4 color;
in  vec2 TextCoord;
uniform sampler2D hrdBuffer;
void main()
{
	//vec3 hdrColor = texture(hrdBuffer, TextCoord).rgb;
	color = texture(hrdBuffer, TextCoord).rgb;
}