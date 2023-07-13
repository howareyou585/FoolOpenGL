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
uniform float shiness;
uniform sampler2D texture_diffuse1;
uniform vec3 eyePos;
uniform vec3 spacular;
uniform Light light;
void main()
{
	vec3 diffuse = texture(texture_diffuse1,texcoord).xyz;
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
	float spacularFator = pow(max(dot(halfDir, nor), 0.0), shiness);
	vec3 spacularColor = light.spacular*spacular*spacularFator;
	vec3 result = ambientColor + diffuseColor + spacularColor;
	color = vec4(result,1.0f);
	
}