#version 330 core
out vec4 color;
in vec2 texcoord;
in vec3 normalDir;
in vec3 fragPos;
struct DirctionLight
{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 spacular;
};

struct HemisphereLight
{
	vec3 direction;
	vec3 skyColor;
	vec3 groundColor;
};

struct Material
{
	sampler2D diffuse;
	sampler2D spacular;
	float shiness;
};


uniform vec3 eyePos;
uniform Material material;

uniform DirctionLight directionlight;
uniform HemisphereLight hemisphereLight;

vec3 calcDirectionLight(DirctionLight dirLight, vec3 nor, vec3 viewDir);
vec3 calcHemisphereLight(HemisphereLight light, vec3 nor);
void main()
{
	vec3 eyeDir = normalize(eyePos-fragPos);
	vec3 nor = normalize(normalDir);
	//vec3 result;
	vec3 result = calcDirectionLight(directionlight, nor,eyeDir);
	result+=calcHemisphereLight(hemisphereLight,nor);
	color = vec4(result,1.0f);
	//color = vec4(1.0f, 0.0f, 0.0f,1.0f);
}
vec3 calcDirectionLight(DirctionLight light, vec3 nor, vec3 viewDir)
{
	vec3 diffuse = texture(material.diffuse,texcoord).xyz;
	
    //ambient
	vec3 ambientColor = light.ambient*diffuse;
	vec3 lightDir =normalize(-light.direction);
	
	//diffuse
	float diffFactor =  max(dot(nor, lightDir), 0.0);
	vec3 diffuseColor = light.diffuse*diffuse*diffFactor;
	
	vec3 halfDir = normalize(viewDir + lightDir);

	//specular
	//ÐÂÔö¾µÃæÌùÍ¼Âß¼­
	vec3 spacular = texture(material.spacular,texcoord).xyz;
	float spacularFator = pow(max(dot(halfDir, nor), 0.0), material.shiness);
	vec3 spacularColor = light.spacular*spacular*spacularFator;
	//
	vec3 outColor = ambientColor + diffuseColor + spacularColor;
	return outColor;
}
vec3 calcHemisphereLight(HemisphereLight light, vec3 nor)
{
	vec3 dirLight =normalize(light.direction);
	float dotNL = dot(dirLight, nor);

	float weight = 0.5f*dotNL+0.5f;
	vec3 outColor = mix(light.groundColor, light.skyColor, weight);
	vec3 diffuse = texture(material.diffuse,texcoord).xyz;
	outColor*=diffuse;
	return outColor;
}
