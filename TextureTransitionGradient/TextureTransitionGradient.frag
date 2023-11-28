#version 430 core

out vec4 FragColor;
in vec2 texCoord;
uniform sampler2D texCat;
uniform sampler2D texDog;
uniform float mixValue;
void main()
{
	vec4 colorCat =  texture(texCat, vec2(texCoord.x, 1- texCoord.y));
	vec4 colorDog = texture(texDog, vec2(texCoord.x, 1- texCoord.y));
	FragColor = colorCat * (1- mixValue) + colorDog*mixValue;
}