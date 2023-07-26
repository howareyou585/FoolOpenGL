#version 330 core

out vec4 color;
in vec2 texcoord;
struct Light
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 spacular;
	float constant;
	float linear;
	float quadratic;
} ;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform vec3 eyePos;
const int NR_LIGHTS = 32;
uniform Light lights[NR_LIGHTS]; 

void main()
{
	vec3 fragPos = texture(gPosition,texcoord).rgb;
	vec3 normalDir =texture(gNormal,texcoord).rgb;
    vec3 nor = normalize(normalDir);
	vec4 albedoSpec = texture(gAlbedoSpec, texcoord);
	vec3 albedo = albedoSpec.rgb;
	float shiness = albedoSpec.a;
	vec3 result;
	for(int i = 0; i <NR_LIGHTS; i++)
	{
		vec3 ambientColor = lights[i].ambient;
	
		vec3 lightDir =normalize(lights[i].position - fragPos);
	
		//diffuse
		float diffFactor =  max(dot(nor, lightDir), 0.0);
		vec3 diffuseColor = lights[i].diffuse*albedo*diffFactor;
		vec3 eyeDir = normalize(eyePos-fragPos);
		vec3 halfDir = normalize(eyeDir + lightDir);

		//specular
		float spacularFator = pow(max(dot(halfDir, nor), 0.0), shiness);
		vec3 spacularColor = lights[i].spacular*albedo*spacularFator;
		result += ambientColor + diffuseColor + spacularColor;

		float dis =  length(lights[i].position - fragPos);
		float attenuation  = 1.0f/(lights[i].constant+lights[i].linear*dis+lights[i].quadratic*dis*dis);
		result *= attenuation;
	}
	//ambient
	

	color = vec4(result,1.0f);
	
}