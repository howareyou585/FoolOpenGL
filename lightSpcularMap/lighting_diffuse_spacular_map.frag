#version 330 core
out vec4 color;
in vec2 texcoord;
in vec3 normalDir;
in vec3 fragPos;
struct Light
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 spacular;
} ;

struct Material
{
	sampler2D diffuse;
	sampler2D spacular;
	float shiness;
};

uniform vec3 eyePos;
uniform Material material;
uniform Light light;
void main()
{
	vec3 diffuse = texture(material.diffuse,texcoord).xyz;
	
    //ambient
	vec3 ambientColor = light.ambient*diffuse;
	vec3 nor = normalize(normalDir);
	vec3 lightDir =normalize(light.position - fragPos);
	
	//diffuse
	float diffFactor =  max(dot(nor, lightDir), 0.0);
	vec3 diffuseColor = light.diffuse*diffuse*diffFactor;
	vec3 eyeDir = normalize(eyePos-fragPos);
	vec3 halfDir = normalize(eyeDir + lightDir);

	//specular
	//����������ͼ�߼�
	vec3 spacular = texture(material.spacular,texcoord).xyz;
	float spacularFator = pow(max(dot(halfDir, nor), 0.0), material.shiness);
	vec3 spacularColor = light.spacular*spacular*spacularFator;
	//
	vec3 result = ambientColor + diffuseColor + spacularColor;
	color = vec4(result,1.0f);
}