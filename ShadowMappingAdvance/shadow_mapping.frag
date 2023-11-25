#version 330 core

out vec4 FragColor;
in VS_OUT
{
	vec3 fragPosWorldSpace;
	vec4 fragPosLightSpace;
	vec3 normalDir;
	vec2 textcoord;
}fs_in;
//方向光
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
uniform int  pcf;
//refBaisRadio 为片元法线与灯光方向的夹角的余弦
//max(0.05 * (1.0 - dot(normal, lightDir)), 0.005)
//当夹角为0时，值最大（1），当夹角为90时 值最小
float calculateShadow(vec4 fragPosLightSpace,float refBaisRadio);

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
	//计算片段是否在阴影中：shadowValue非0即1
	float shadowValue =  calculateShadow(fs_in.fragPosLightSpace,dot(nor, lightDir));
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
float calculateShadow(vec4 fragPosLightSpace,float refBaisRadio)
{
	// perform perspective divide
	vec3 projCoord = fragPosLightSpace.xyz/fragPosLightSpace.w;//在灯光空间下片元的屏幕坐标
	// transform to [0,1] range
	projCoord = projCoord*0.5+0.5;
	 // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
	// max(0.05 * (1.0 - dot(normal, lightDir)), 0.005)
	float relBais = max(0.05*(1- refBaisRadio),bais);
	//采样点在深度贴图中的最近距离
	//vec2 ts = 1.0f/textureSize(shadowMap,0);
	float currentDepth = projCoord.z;//在灯光空间下片元的屏幕坐标的深度

	//vec2 texsize = textureSize(shadowMap,0);

	
	float shadow  =0;
	if(pcf == 0)
	{
		float closestDepth = texture(shadowMap, projCoord.xy ).r;
		shadow = currentDepth-relBais>closestDepth ? 1.0f : 0.0f;
	}
	else
	{
		// PCF
		vec2 txtSize = 1.0 / textureSize(shadowMap,0);
		for(int x = -pcf; x<=pcf;x++)
		{
			for(int y = -pcf; y<= pcf;y++)
			{
				vec2 offset= vec2(x*txtSize.x,y*txtSize.y);
				float closestDepth = texture(shadowMap, projCoord.xy +  offset.xy).r;
				shadow += currentDepth-relBais>closestDepth ? 1.0f : 0.0f;
			}
		}
		shadow/=(pcf*pcf);
	}
	
	
	// get depth of current fragment from light's perspective
	
	//比较采样点在深度贴图中的深度值与采样点在本次采样的深度值。要么在阴影中，要么不在阴影中
	if(projCoord.z>1.0)
	{
		shadow = 0.0f;
	}
	
	return shadow;
}