#version 330
struct DirectionLight //Æ½ÐÐ¹â
{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 spcular;
};
struct Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 spcular;
};
in vec3 Normal;
in vec2 TextCoord;
in vec3 FragPos;
uniform sampler2D texture_diffuse1;
uniform DirectionLight light;
uniform Material material;
out vec4 color;
vec3 CalcDirectionLight(DirectionLight dirLight, vec3 normal, vec3 viewDir);
void main()
{
	//vec3 ambient = light.ambient* material.ambient;
	//vec3 diffuse = light.diffuse* texture(texture_diffuse1,TextCoord).rgb;
	//vec3 speculer = light.spcular*material.ambient;
	//vec3 outColor = CalcDirectionLight(light,
	//color = texture(tex,vec2(TextCoord.s, 1.0-TextCoord.t));
	//color = vec4(ambient + diffuse + speculer, 1.0f);
	color = texture(texture_diffuse1, TextCoord);
}
vec3 CalcDirectionLight(DirectionLight dirLight, vec3 normal, vec3 viewDir)
{
	vec3 outColor ;

	return outColor;
}