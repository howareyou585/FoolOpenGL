#version 430 core

out vec4 FragColor;
in vec2 texCoord1;
in vec2 texCoord2;
uniform sampler2D texBricks;
uniform sampler2D texGrass;

void main()
{
	vec4 colorBricks =  texture(texBricks, vec2(texCoord1.x, 1- texCoord1.y));
	vec4 colorGrass= texture(texGrass, vec2(texCoord2.x, 1- texCoord2.y));
	FragColor = colorBricks*(1-colorGrass.a) + colorGrass*colorGrass.a;

}