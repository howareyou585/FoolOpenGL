#version 330 core

out vec4 FragColor;
in VS_OUT
{
	vec3 fragPosWorldSpace;
	vec4 fragPosLightSpace;
	vec3 normalDir;
	vec2 textcoord;
}fs_in;
//�����
struct DirectionLight
{
	vec3 direction;
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 spacular;
} ;

struct Material
{
	sampler2D diffuseTexture;
	float shiness;
};

uniform vec3 eyePos;
uniform Material material;
uniform DirectionLight light;
uniform sampler2D shadowMap;
uniform bool bDirection;
uniform float bais;
float calculateShadow(vec4 fragPosLightSpace);

void main()
{
	vec3 color = texture(material.diffuseTexture, fs_in.textcoord).xyz;
    //ambient
	vec3 ambientColor = light.ambient*color;
	vec3 nor = normalize(fs_in.normalDir);
	vec3 lightDir = vec3(0.5,0.5,0.5);
	if(!bDirection)
	{
		lightDir = normalize(light.position - fs_in.fragPosWorldSpace);
	}
	//vec3 lightDir = normalize(light.position - vec3(0,0,0));
	//����Ƭ���Ƿ�����Ӱ�У�shadowValue��0��1
	float shadowValue =  calculateShadow(fs_in.fragPosLightSpace);
	//diffuse
	float diffFactor =  max(dot(nor, lightDir), 0.0);
	vec3 diffuseColor = light.diffuse*color*diffFactor;
	//
	diffuseColor = diffuseColor*(1-shadowValue);
	vec3 eyeDir = normalize(eyePos-fs_in.fragPosWorldSpace);
	vec3 halfDir = normalize(eyeDir + lightDir);

	//specular
	float spacularFator = pow(max(dot(halfDir, nor), 0.0), material.shiness);
	vec3 spacularColor = light.spacular*color*spacularFator;
	spacularColor = spacularColor*(1-shadowValue);
	vec3 result = ambientColor + diffuseColor + spacularColor;
	FragColor = vec4(result,1.0f);


}
float calculateShadow(vec4 fragPosLightSpace)
{
	// perform perspective divide
	vec3 projCoord = fragPosLightSpace.xyz/fragPosLightSpace.w;//�ڵƹ�ռ���ƬԪ����Ļ����
	// transform to [0,1] range
	projCoord = projCoord*0.5+0.5;
	 // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
	//�������������ͼ�е��������
	float closestDepth = texture(shadowMap, projCoord.xy).r;
	// get depth of current fragment from light's perspective
	float currentDepth = projCoord.z;//�ڵƹ�ռ���ƬԪ����Ļ��������
	//�Ƚϲ������������ͼ�е����ֵ��������ڱ��β��������ֵ��Ҫô����Ӱ�У�Ҫô������Ӱ��
	float shadow = currentDepth-bais>closestDepth ? 1.0f : 0.0f;
	return shadow;
}