#version 330

out vec4 FragColor;
in vec2 texCoord;
uniform sampler2DArray textureArray;
void main()
{
	
	FragColor = texture(textureArray, vec3(texCoord,1));
}