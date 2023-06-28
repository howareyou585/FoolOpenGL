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
struct PointLight
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 spacular;
	float constant;
	float linear;
	float quadratic;
} ;

struct SpotLight
{
	vec3 position;
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 spacular;
	float cutoff;//计算完cos后的值
	float outerCutoff;//计算完cos后的值
	float constant;
	float linear;
	float quadratic;
};
struct Material
{
	sampler2D diffuse;
	sampler2D spacular;
	float shiness;
};
#define POINT_NUM 4
uniform vec3 eyePos;
uniform Material material;
uniform PointLight pointlights[POINT_NUM];
uniform SpotLight  spotlight;
uniform DirctionLight directionlight;
vec3 calcDirectionLight(DirctionLight dirLight, vec3 nor, vec3 viewDir);
vec3 calcSpotLight(SpotLight spotLight, vec3 nor, vec3 viewDir);
vec3 calcPointLight(PointLight pntLight, vec3 nor, vec3 viewDir);
void main()
{
	vec3 eyeDir = normalize(eyePos-fragPos);
	vec3 nor = normalize(normalDir);
	vec3 result = calcDirectionLight(directionlight, nor,eyeDir);
	
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
	//新增镜面贴图逻辑
	vec3 spacular = texture(material.spacular,texcoord).xyz;
	float spacularFator = pow(max(dot(halfDir, nor), 0.0), material.shiness);
	vec3 spacularColor = light.spacular*spacular*spacularFator;
	//
	vec3 outColor = ambientColor + diffuseColor + spacularColor;
	return outColor;
}
vec3 calcSpotLight(SpotLight spotLight, vec3 nor, vec3 viewDir)
{
	vec3 outColor;
	return outColor;
}
vec3 calcPointLight(PointLight pntLight, vec3 nor, vec3 viewDir)
{
	vec3 outColor;
	return outColor;
}