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
	vec3 lightColor;
};
uniform Material material;
uniform Light light;

void main()
{
	vec3 ambientColor = light.lightColor*material.ambient;
	vec3 lightDir = normalize(light.positon - fragPos);
	vec3 diffuseColor = light.lightColor*max(dot(lightDir, normalize(normalDir)),0.f)*material.diffuse;
	vec3 viewDir = normalize(viewPos-fragPos);
	vec3 halfWayDir =normalize(lightDir+viewDir);
	float specVal = pow(max(dot(normalize(normalDir),halfWayDir),0),material.shininess); 
	vec3 specularColor = light.lightColor*specVal*material.specular;
	//vec3 specularColor = light.lightColor* dot();
	color = vec4(ambientColor + diffuseColor + specularColor,1.0f);
}