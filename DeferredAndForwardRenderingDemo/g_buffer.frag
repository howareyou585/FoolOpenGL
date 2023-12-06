#version 330 core

layout(location = 0) out vec3 postionColor;
layout(location = 1) out vec3 normalColor;
layout(location = 2) out vec4 albedoSpecColor;
in vec3 normal;
in vec3 fragPos;
in vec2 texCoord;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_spacular1;
void main()
{
	postionColor = fragPos;
	normalColor = normal;
	albedoSpecColor.rgb = texture(texture_diffuse1,texCoord).rgb;
	albedoSpecColor.a = texture(texture_spacular1,texCoord).r;
	
}