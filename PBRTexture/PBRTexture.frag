#version 330

out vec4 fragColor;
uniform vec3 camPos;
uniform sampler2D albedoMap;
uniform sampler2D roughnessMap;
uniform sampler2D metallicMap;
uniform sampler2D normalMap;
uniform sampler2D aoMap;
uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];
in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;
const float PI =3.14159265359;

// ----------------------------------------------------------------------------
// Easy trick to get tangent-normals to world-space to keep PBR code simplified.
// Don't worry if you don't get what's going on; you generally want to do normal 
// mapping the usual way for performance anways; I do plan make a note of this 
// technique somewhere later in the normal mapping tutorial.
vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(normalMap, TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(WorldPos);
    vec3 Q2  = dFdy(WorldPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N   = normalize(Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

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

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
void main()
{
	vec3 diffuse = texture(albedoMap,TexCoords).rgb;
	diffuse     = pow(diffuse.rgb, vec3(2.2));
	float metallic = texture(metallicMap,TexCoords).r;
	float roughness = texture(roughnessMap,TexCoords).r;
	float ao        = texture(aoMap, TexCoords).r;
	vec3 Normal = getNormalFromMap();
	// calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow) 
	//计算法向入射时的反射率；
	//如果金属度为0，则F0 = 0.04，如果金属度为1，diffuse颜色作为F0
	vec3 F0 = vec3(0.04); // 初始化0.04 
	F0 = mix(F0, diffuse, metallic);

	vec3 nDir = normalize(Normal) ; // 法线向量
	vec3 vDir = normalize(camPos- WorldPos);//
	vec3 BRDF=vec3(0,0,0);
	for(int i = 0; i < 4; i++)
	{
		vec3 lDir = normalize(lightPositions[i] - WorldPos); // 灯光的方向
		vec3 hDir = normalize(vDir + lDir);//半程向量
		float distance = length(lightPositions[i] - WorldPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = lightColors[i] * attenuation;
	
		//高光的计算
		float dGGX_result =  DistributionGGX(nDir,hDir, roughness); 
		float ggx_result = GeometrySmith(nDir,  vDir,  lDir, roughness);
		vec3  fresnel = fresnelSchlick(clamp(dot(hDir, vDir), 0.0, 1.0), F0);
		// Cook-Torrance BRDF
		vec3 numerator    = dGGX_result * ggx_result * fresnel; //分子
		float denominator = 4.0 * max(dot(nDir, vDir), 0.0) * max(dot(nDir, lDir), 0.0) + 0.0001; // 分母 =》+ 0.0001 to prevent divide by zero
		vec3 specular = numerator / denominator;

		// kS is equal to Fresnel
		vec3 kS = fresnel;
		// for energy conservation, the diffuse and specular light can't
		// be above 1.0 (unless the surface emits light); to preserve this
		// relationship the diffuse component (kD) should equal 1.0 - kS.
		// 能量守恒，漫射光和镜面光不能大于1.0（除非表面发光）；
		//diffuse分量（kD）应等于1.0-kS。
		vec3 kD = vec3(1.0) - kS;
		// multiply kD by the inverse metalness such that only non-metals 
		// have diffuse lighting, or a linear blend if partly metal (pure metals
		// have no diffuse light).
		//将kD乘以(1-金属性)，使得只有非金属具有漫射照明，或者如果部分为金属（纯金属没有漫射光）
		kD *= 1.0 - metallic;	  
		float NdotL = max(0, dot(nDir,lDir));
		BRDF += (kD*diffuse/PI+specular)*NdotL*radiance;
	}
	
	//现在已经有了漫反射和高光，没有环境光的话，这两种光作用不到的话，会出现黑的情况。
	vec3 ambient = diffuse*0.05*ao;
	vec3 color = ambient+BRDF;
	// HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2)); 
	
	fragColor = vec4(color, 1.0);
}