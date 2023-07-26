#version 330 core

layout(location = 0) out vec4 postionColor;
layout(location = 1) out vec4 normalColor;
layout(location = 2) out vec4 albedoSpecColor;
in vec3 normal;
in vec3 fragPos;
in vec2 texCoord;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_spacular1;
void main()
{
	postionColor = vec4(fragPos,1.0);
	normalColor = vec4(normal,1.0);
	vec3 diffuseColor = texture(texture_diffuse1,texCoord).rgb;
	float shiness = texture(texture_spacular1,texCoord).r;
	albedoSpecColor = vec4(diffuseColor,shiness);
}