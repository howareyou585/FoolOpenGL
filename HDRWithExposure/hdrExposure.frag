#version 330

out vec4 FragColor;
in vec2 texCoord;
uniform sampler2D hdrBuffer;
uniform float exposure;
uniform bool hdr;
void main()
{
	FragColor = texture(hdrBuffer,texCoord);
}