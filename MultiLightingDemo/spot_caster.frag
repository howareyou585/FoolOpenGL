#version 330 core
out vec4 color;
in vec2 texcoord;
in vec3 normalDir;
in vec3 fragPos;
struct Spotlight
{
	vec3 position; //聚光灯的位置
	vec3 direction;//聚光灯的朝向
	vec3 ambient;
	vec3 diffuse;
	vec3 spacular;
	float cutoff;//计算完cos后的值
	
} ;

struct Material
{
	sampler2D diffuse;
	sampler2D spacular;
	float shiness;
};

uniform vec3 eyePos;
uniform Material material;
uniform Spotlight light;
void main()
{
	
	vec3 lightDir =normalize(light.position - fragPos);
	vec3 revLightDir = -lightDir;
	vec3 direction = normalize(light.direction);
	float theta = dot(revLightDir, direction);
	vec3 mDiffuse = texture(material.diffuse,texcoord).xyz;
    //ambient
	vec3 ambientColor = light.ambient*mDiffuse;
	if(theta>light.cutoff)
	{
	vec3 nor = normalize(normalDir);
	//diffuse
	float diffFactor =  max(dot(nor, lightDir), 0.0);
	vec3 diffuseColor = light.diffuse*mDiffuse*diffFactor;
	vec3 eyeDir = normalize(eyePos-fragPos);
	vec3 halfDir = normalize(eyeDir + lightDir);

	//specular
	float spacularFator = pow(max(dot(halfDir, nor), 0.0), material.shiness);
	vec3 mSpacular = vec3(texture(material.spacular, texcoord));
	vec3 spacularColor = light.spacular*mSpacular*spacularFator;
	vec3 result = ambientColor + diffuseColor + spacularColor;
	color = vec4(result,1.0f);
	}
	else
	{
		vec3 result = ambientColor;
		color = vec4(result,1.0f);
	}
}