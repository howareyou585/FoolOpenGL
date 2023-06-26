#version 330 core
in VS_OUT
{
	vec3 fragPosWorldSpace;
	vec4 fragPosLightSpace;
	vec3 normalDir;
	vec2 textcoord;
}fs_in;
//·½Ïò¹â
struct DirectionLight
{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 spacular;
} ;

struct Material
{
	sampler2D diffuseTexture;
	vec3 spacular;
	float shiness;
};

uniform vec3 eyePos;
uniform Material material;
uniform DirectionLight light;
uniform sampler2D shadowMap;
float calculateShadow(vec4 fragPosLightSpace);
out vec4 color;
void main()
{
	vec3 diffuse = texture(material.diffuseTexture, fs_in.textcoord).xyz;
    //ambient
	vec3 ambientColor = light.ambient*diffuse;
	vec3 nor = normalize(fs_in.normalDir);
	vec3 lightDir =normalize(light.direction);
	
	//diffuse
	float diffFactor =  max(dot(nor, lightDir), 0.0);
	vec3 diffuseColor = light.diffuse*diffuse*diffFactor;
	vec3 eyeDir = normalize(eyePos-fs_in.fragPosWorldSpace);
	vec3 halfDir = normalize(eyeDir + lightDir);

	//specular
	float spacularFator = pow(max(dot(halfDir, nor), 0.0), material.shiness);
	vec3 spacularColor = light.spacular*material.spacular*spacularFator;
	vec3 result = ambientColor + diffuseColor + spacularColor;
	float shadowValue =  calculateShadow(fs_in.fragPosLightSpace);
	result = result*shadowValue;
	color = vec4(result,1.0f);
}
float calculateShadow(vec4 fragPosLightSpace)
{
	// perform perspective divide
	vec3 projCoord = fragPosLightSpace.xyz/fragPosLightSpace.w;
	// transform to [0,1] range
	projCoord = projCoord*0.5+0.5;
	 // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
	float closestDepth = texture(shadowMap, fs_in.textcoord).r;
	// get depth of current fragment from light's perspective
	float currentDepth = projCoord.z;
	float shadow = currentDepth>closestDepth ? 1.0f : 0.0f;
	return shadow;
}