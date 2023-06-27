#version 330

out vec4 FragColor;
in vec2 texCoord;
uniform sampler2D s_texture;
uniform sampler2D s_texture2;
void main()
{
	
	FragColor = mix(texture(s_texture, texCoord),texture(s_texture2, texCoord),0.2);
}