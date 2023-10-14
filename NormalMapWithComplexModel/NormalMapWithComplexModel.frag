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


uniform float shiness;
uniform Light light;

uniform sampler2D texture_diffuse1; // 漫反射
uniform sampler2D texture_specular1; //高光
uniform sampler2D texture_normal1;  // 发线
void main()
{
	vec3 diffuse = texture(texture_diffuse1,fs_in.texcoord).xyz;
	 // obtain normal from normal map in range [0,1]
	vec3 normalDir = texture(texture_normal1,fs_in.texcoord).xyz;
	// transform normal vector to range [-1,1]
	normalDir = normalize(normalDir*2-1);
	
    //ambient
	vec3 ambientColor = light.ambient*diffuse;

	vec3 lightDir = normalize(fs_in.tangentLightPos - fs_in.tangentFragPos);
	
	//diffuse
	float diffFactor =  max(dot(normalDir, lightDir), 0.0);
	vec3 diffuseColor = light.diffuse*diffuse*diffFactor;
	
	vec3 eyeDir = normalize(fs_in.tangentViewPos-fs_in.tangentFragPos);
	vec3 halfDir = normalize(eyeDir + lightDir);

	//specular
	vec3 spacular = texture(texture_specular1,fs_in.texcoord).xyz;
	
	float spacularFator = pow(max(dot(halfDir, normalDir), 0.0), shiness);
	vec3 spacularColor = light.spacular*spacular*spacularFator;
	vec3 result = ambientColor + diffuseColor + spacularColor;
	color = vec4(result,1.0f);
}