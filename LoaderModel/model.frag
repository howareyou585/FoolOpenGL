#version 330 core
out vec4 color;
in vec2 texcoord;
in vec3 normalDir;
in vec3 fragPos;
//uniform sampler2D t0;
//uniform sampler2D t1;
//uniform sampler2D t2;

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

uniform DirctionLight directionlight;
uniform PointLight pointlights[POINT_NUM];
uniform SpotLight  spotlight;
uniform bool  enableAttenuation;

vec3 calcDirectionLight(DirctionLight dirLight, vec3 nor, vec3 viewDir);
vec3 calcSpotLight(SpotLight spotLight, vec3 nor, vec3 fragPos, vec3 viewDir);
vec3 calcPointLight(PointLight pntLight, vec3 nor, vec3 fragPos, vec3 viewDir);
void main()
{
	vec3 eyeDir = normalize(eyePos-fragPos);
	vec3 nor = normalize(normalDir);
	//vec3 result;
	vec3 result = calcDirectionLight(directionlight, nor,eyeDir);
	for(int i = 0; i <POINT_NUM; i++)
	{
		result+= calcPointLight(pointlights[i],nor,fragPos,eyeDir);
	}
	result+= calcSpotLight(spotlight,nor, fragPos, eyeDir);
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
vec3 calcSpotLight(SpotLight light, vec3 nor, vec3 fragPos,vec3 viewDir)
{
	vec3 outColor;
	vec3 lightDir =normalize(light.position - fragPos);
	vec3 revLightDir = -lightDir;
	vec3 direction = normalize(light.direction); //聚光灯的朝向
	float theta = dot(revLightDir, direction);//聚光灯的朝向与该点像素的光照方向的夹角的余弦
	vec3 mDiffuse = texture(material.diffuse,texcoord).xyz;
	//计算强度
	float intensity = clamp((theta - light.outerCutoff)/(light.cutoff - light.outerCutoff),0.0f,1.0f);
    //ambient
	vec3 ambientColor = light.ambient*mDiffuse;
	if(theta>light.outerCutoff)
	{
	vec3 nor = normalize(normalDir);
	//diffuse
	float diffFactor =  max(dot(nor, lightDir), 0.0);
	vec3 diffuseColor = light.diffuse*mDiffuse*diffFactor;
	
	vec3 halfDir = normalize(viewDir + lightDir);

	//specular
	float spacularFator = pow(max(dot(halfDir, nor), 0.0), material.shiness);
	vec3 mSpacular = vec3(texture(material.spacular, texcoord));
	vec3 spacularColor = light.spacular*mSpacular*spacularFator;
	vec3 result = ambientColor + diffuseColor*intensity + spacularColor*intensity;
	//计算衰减值
	if(enableAttenuation)
	{
		float distance =  length(light.position - fragPos);
		float attenuation  = 1.0f/(light.constant+light.linear*distance+light.quadratic*distance*distance);
		result *= attenuation;
	}
	outColor = result;
	}
	else
	{
		outColor  = ambientColor;
	}
	return outColor;
}
vec3 calcPointLight(PointLight light, vec3 nor, vec3 fragPos, vec3 viewDir)
{
	vec3 diffuse = texture(material.diffuse,texcoord).xyz;
	
    //ambient
	vec3 ambientColor = light.ambient*diffuse;
	vec3 lightDir =normalize(light.position - fragPos);
	
	//diffuse
	float diffFactor =  max(dot(nor, lightDir), 0.0);
	vec3 diffuseColor = light.diffuse*diffuse*diffFactor;
	vec3 halfDir = normalize(viewDir + lightDir);

	//specular
	//新增镜面贴图逻辑
	vec3 spacular = texture(material.spacular,texcoord).xyz;
	float spacularFator = pow(max(dot(halfDir, nor), 0.0), material.shiness);
	vec3 spacularColor = light.spacular*spacular*spacularFator;
	//计算衰减值
	float attenuation = 1.0f;
	if(enableAttenuation)
	{
		float distance =  length(light.position - fragPos);
		attenuation  = 1.0f/(light.constant+light.linear*distance+light.quadratic*distance*distance);
	}
	vec3 outColor = ambientColor*attenuation + diffuseColor* attenuation+ spacularColor*attenuation;
	return outColor;
}