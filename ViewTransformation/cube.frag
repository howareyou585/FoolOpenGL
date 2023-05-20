#version 330
struct DirectionLight //Æ½ÐÐ¹â
{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 spcular;
};
in vec3 Normal;
in vec2 TextCoord;
in vec3 FragPos;
uniform DirectionLight light;
uniform sampler2D texture_diffuse1;
out vec4 color;
vec3 CalcDirectionLight(DirectionLight dirLight, vec3 normal, vec3 viewDir);
void main()
{
	//vec3 outColor = CalcDirectionLight(light,
	//color = texture(tex,vec2(TextCoord.s, 1.0-TextCoord.t));
	color = texture(texture_diffuse1,TextCoord);
}
vec3 CalcDirectionLight(DirectionLight dirLight, vec3 normal, vec3 viewDir)
{
	vec3 outColor ;

	return outColor;
}