#version 330

out vec4 color;
uniform vec3 camPos;
uniform float _roughness;
uniform float _metallic;
uniform vec3 lightPositions;
uniform vec3 lightColors;
in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;
const float PI =3.14159265359;
//法向发布函数
//N 法线方向
//H 半程向量
//roughness 粗糙度
float DistributionGGX(vec3 N, vec3 H, float roughness) 
{
	float a = roughness*roughness;
	float square_a = a*a;
	float NdotH = max(dot(N,H),0.0);
	float square_NdotH = NdotH*NdotH;

	float nom = square_a;//分子
	float denom = (square_NdotH * (square_a - 1.0) + 1.0);
	denom = PI * denom * denom; //分母
	return nom/denom;
}

// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}


void main()
{
	vec3 nDir = normalize(Normal) ; // 法线向量
	vec3 vDir = normalize(camPos- WorldPos);//
	vec3 lDir = normalize(lightPositions - WorldPos); // 灯光的方向
    vec3 hDir = normalize(vDir + lDir);//半程向量

	float dGGX_result =  DistributionGGX(nDir,hDir, _roughness);
	
	float ggx_result = GeometrySmith(nDir,  vDir,  lDir, _roughness);
	float spacular = dGGX_result* ggx_result;
	color = vec4(spacular,spacular,spacular, 1.0);
}