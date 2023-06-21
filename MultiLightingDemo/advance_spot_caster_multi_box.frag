#version 330 core
out vec4 color;
in vec2 texcoord;
in vec3 normalDir;
in vec3 fragPos;
struct Spotlight
{
	vec3 position; //聚光灯的位置
	vec3 direction;//聚光灯的朝向
	vec3 ambient;
	vec3 diffuse;
	vec3 spacular;
	float cutoff;//计算完cos后的值
	float outerCutoff;//计算完cos后的值
} ;

struct Material
{
	sampler2D diffuse;
	sampler2D spacular;
	float shiness;
};

uniform vec3 eyePos;
uniform Material material;
uniform Spotlight light;
void main()
{
	
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
	vec3 eyeDir = normalize(eyePos-fragPos);
	vec3 halfDir = normalize(eyeDir + lightDir);

	//specular
	float spacularFator = pow(max(dot(halfDir, nor), 0.0), material.shiness);
	vec3 mSpacular = vec3(texture(material.spacular, texcoord));
	vec3 spacularColor = light.spacular*mSpacular*spacularFator;
	vec3 result = ambientColor + diffuseColor*intensity + spacularColor*intensity;
	
	color = vec4(result,1.0f);
	}
	else
	{
		vec3 result = ambientColor;
		color = vec4(result,1.0f);
	}
}