#version 330 core
out vec4 color;
in VS_OUT
{
   vec2 texcoord;
   vec3 tangentFragPos; // 切线空间的顶点
   vec3 tangentLightPos;// 切线空间的灯光位置
   vec3 tangentViewPos; // 切线空间的相机位置
  
}fs_in;
struct Light
{
	vec3 ambient;
	vec3 diffuse;
	vec3 spacular;
} ;

struct Material
{
	sampler2D diffuse;
	vec3 spacular;
	float shiness;
};

uniform Material material;
uniform Light light;
uniform sampler2D normalMap;
uniform sampler2D depthMap;
uniform float heightScale;

vec2 parallaxMapping(vec2 texCoords, vec3 viewDir)
{ 
    // number of depth layers
    const float numLayers = 10;
    // calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    // depth of current layer
    float currentLayerDepth = 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = viewDir.xy * heightScale; 
    vec2 deltaTexCoords = P / numLayers;

	vec2  currentTexCoords     = texCoords;
	float currentDepthMapValue = texture(depthMap, currentTexCoords).r;

	while(currentLayerDepth < currentDepthMapValue)
	{
		// shift texture coordinates along direction of P
		currentTexCoords -= deltaTexCoords;
		// get depthmap value at current texture coordinates
		currentDepthMapValue = texture(depthMap, currentTexCoords).r;  
		// get depth of next layer
		currentLayerDepth += layerDepth;  
	}

	return currentTexCoords;   
}

void main()
{
	vec3 eyeDir = normalize(fs_in.tangentViewPos-fs_in.tangentFragPos);
	vec2 texCoords = parallaxMapping(fs_in.texcoord, eyeDir);
	if(texCoords.x > 1.0f || texCoords.y > 1.0f || texCoords.x <0.0 || texCoords.y<0.0)
	{
		discard;
	}
	vec3 diffuse = texture(material.diffuse,texCoords).xyz;
	 // obtain normal from normal map in range [0,1]
	vec3 normalDir = texture(normalMap, texCoords).xyz;
	// transform normal vector to range [-1,1]
	normalDir = normalize(normalDir*2-1);
	
	
    //ambient
	vec3 ambientColor = light.ambient*diffuse;

	vec3 lightDir = normalize(fs_in.tangentLightPos - fs_in.tangentFragPos);
	
	//diffuse
	float diffFactor =  max(dot(normalDir, lightDir), 0.0);
	vec3 diffuseColor = light.diffuse*diffuse*diffFactor;
	
	
	vec3 halfDir = normalize(eyeDir + lightDir);

	//specular
	float spacularFator = pow(max(dot(halfDir, normalDir), 0.0), material.shiness);
	vec3 spacularColor = light.spacular*material.spacular*spacularFator;
	vec3 result = ambientColor + diffuseColor + spacularColor;
	color = vec4(result,1.0f);
}