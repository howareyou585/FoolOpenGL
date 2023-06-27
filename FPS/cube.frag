#version 330

out vec4 FragColor;
in vec2 texCoord;
uniform sampler2D s_texture;
void main()
{
	
	FragColor = texture(s_texture, texCoord);
}