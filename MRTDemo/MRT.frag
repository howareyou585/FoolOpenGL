#version 330 core
out vec4 color;
in vec2 texCoord;
uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform sampler2D tex3;

void main()
{
	if(texCoord.x<0.5 && texCoord.y<0.5)
	{
		color = texture(tex0,texCoord); //Ô­É«
	}
	else if(texCoord.x>0.5 && texCoord.y<0.5)
	{
		color = texture(tex1,texCoord); //ºì
	}
	else if(texCoord.x<0.5 && texCoord.y>0.5)
	{
		color = texture(tex2,texCoord); //ÂÌ
	}
	else //if(texCoord.x>0.5 && texCoord.y>0.5)
	{
		color = texture(tex3,texCoord);//À¶
	}
}