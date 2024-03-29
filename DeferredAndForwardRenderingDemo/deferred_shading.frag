#version 330 core

out vec4 fragColor;
in vec2 texcoord;
struct Light
{
	vec3 position;
	vec3 color;
	//vec3 ambient;
	//vec3 diffuse;
	//vec3 spacular;
	float constant;
	float linear;
	float quadratic;
	float radius; //������İ뾶
} ;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform vec3 eyePos;
const int NR_LIGHTS = 32;
uniform Light lights[NR_LIGHTS]; 
uniform bool lightVolumeState;

void main()
{
	vec3 fragPos = texture(gPosition,texcoord).rgb;
	vec3 normalDir =texture(gNormal,texcoord).rgb;
    vec3 nor = normalize(normalDir);
	vec4 albedoSpec = texture(gAlbedoSpec, texcoord);
	vec3 diffuse = albedoSpec.rgb;
	vec3 result = diffuse *0.1;
	float specular = albedoSpec.a;

	vec3 eyeDir = normalize(eyePos-fragPos);
	for(int i = 0; i <NR_LIGHTS; i++)
	{
		//vec3 ambientColor = lights[i].ambient;
		float dis =  length(lights[i].position - fragPos);
		if(!lightVolumeState  || lights[i].radius<dis)
		{
		vec3 lightDir =normalize(lights[i].position - fragPos);
	
		//diffuse
		float diffFactor =  max(dot(nor, lightDir), 0.0);
		vec3 diffuseColor = lights[i].color*diffuse*diffFactor;
	
		vec3 halfDir = normalize(eyeDir + lightDir);

		//specular
		float spacularFator = pow(max(dot(halfDir, nor), 0.0), 16);
		vec3 spacularColor = lights[i].color*specular*spacularFator;

		
		
		float attenuation  = 1.0f/(lights[i].constant+lights[i].linear*dis+lights[i].quadratic*dis*dis);
		
		result += ( diffuseColor + spacularColor)*attenuation;
		}
	}

	fragColor = vec4(result,1.0f);
}