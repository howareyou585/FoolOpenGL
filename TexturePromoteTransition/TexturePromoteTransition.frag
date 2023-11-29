#version 430 core

out vec4 FragColor;
in vec2 texCoord;
uniform sampler2D texCat;
uniform sampler2D texDog;
uniform float offset;
void main()
{
	vec4 tmpColor= vec4(0,0,0,1);
	if(texCoord.x<=offset)
	{
		tmpColor = texture(texCat,vec2(texCoord.x+(1-offset) , 1- texCoord.y));
	}
	else
	{
		tmpColor = texture(texDog,vec2(texCoord.x-offset , 1- texCoord.y));
	}
	FragColor = tmpColor;
}