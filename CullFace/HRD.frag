#version 330 core

out vec4 color;
in  vec2 TextCoord;
uniform sampler2D hrdBuffer;
void main()
{
	//vec3 hdrColor = texture(hrdBuffer, TextCoord).rgb;
	color = vec4(texture(hrdBuffer, TextCoord).rgb,1.0f);
}