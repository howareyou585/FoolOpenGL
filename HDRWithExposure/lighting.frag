#version 330 core
out vec4 color;
in vec2 texcoord;
in vec3 normalDir;
in vec3 fragPos;

struct PointLight
{
	vec3 position;
    vec3 color;
} ;

uniform sampler2D diffuseMap;

#define POINT_NUM 4

uniform vec3 eyePos;


uniform PointLight pointlights[POINT_NUM];
vec3 calcPointLight(PointLight pntLight, vec3 nor, vec3 fragPos, vec3 viewDir);
void main()
{
	vec3 eyeDir = normalize(eyePos-fragPos);
	vec3 nor = normalize(normalDir);
	vec3 result;
	
	for(int i = 0; i <POINT_NUM; i++)
	{
		result+= calcPointLight(pointlights[i],nor,fragPos,eyeDir);
	}
	color = vec4(result,1.0f);
}


vec3 calcPointLight(PointLight light, vec3 nor, vec3 fragPos, vec3 viewDir)
{
	vec3 diffuse = texture(diffuseMap,texcoord).xyz;

	vec3 lightDir =normalize(light.position - fragPos);
	
	//diffuse
	float diffFactor =  max(dot(nor, lightDir), 0.0);
	vec3 diffuseColor = light.color*diffuse*diffFactor;

	float distance =  length(light.position - fragPos);
	
	vec3 outColor = diffuseColor*(1.0f/(distance*distance));
	return outColor;
}