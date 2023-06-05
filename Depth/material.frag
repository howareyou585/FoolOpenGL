#version 330 core
out vec4 color;
in vec2 uvCoord;
in vec3 fragPos;
in vec3 normalDir;

uniform sampler2D tex;
uniform vec3 viewPos;
struct Material 
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};
struct Light
{
	vec3 positon;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

};
uniform Material material;
uniform Light light;

void main()
{
	vec3 normal =  normalize(normalDir);
	//ambient
	vec3 ambientColor = light.ambient*material.ambient;
	//diffuse
	vec3 lightDir = normalize(light.positon - fragPos);
	vec3 diffuseColor = light.diffuse*max(dot(lightDir,normal),0.f)*material.diffuse;
	//specular
	vec3 viewDir = normalize(viewPos-fragPos);
	vec3 halfWayDir =normalize(lightDir+viewDir);
	float specVal = pow(max(dot(normal,halfWayDir),0),material.shininess); 
	vec3 specularColor = light.specular*specVal*material.specular;
	vec3 result = ambientColor + diffuseColor + specularColor;
	color = vec4(result,1.0f);
}