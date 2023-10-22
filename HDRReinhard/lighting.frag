#version 330 core
out vec4 FragColor;
in vec2 texcoord;
in vec3 normalDir;
in vec3 fragPos;

struct PointLight
{
	vec3 position;
    vec3 color;
} ;

uniform sampler2D diffuseMap;

#define POINT_NUM 4

uniform PointLight pointlights[POINT_NUM];
vec3 calcPointLight(PointLight pntLight, vec3 nor, vec3 fragPos);
void main()
{
	vec3 normal = normalize(normalDir);
	 // ambient
	vec3 color = texture(diffuseMap,texcoord).rgb;
    vec3 ambient = 0.0 * color;
    // lighting
    vec3 lighting = vec3(0.0);
	
	for(int i = 0; i <POINT_NUM; i++)
	{
		// diffuse
        vec3 lightDir = normalize(pointlights[i].position - fragPos);
        float diff = max(dot(lightDir, normal), 0.0);
        vec3 diffuse = pointlights[i].color * diff * color;      
        vec3 result = diffuse;        
        // attenuation (use quadratic as we have gamma correction)
        float distance = length(fragPos - pointlights[i].position);
        result *= 1.0 / (distance * distance);
        lighting += result;
	}
	FragColor = vec4(ambient + lighting,1.0f);
}


vec3 calcPointLight(PointLight light, vec3 nor, vec3 fragPos)
{
	vec3 diffuse = texture(diffuseMap,texcoord).xyz;

	vec3 lightDir =normalize(light.position - fragPos);
	
	//diffuse
	float diffFactor =  max(dot(nor, lightDir), 0.0);
	vec3 diffuseColor = light.color*diffuse*diffFactor;

	float distance =  length(light.position - fragPos);
	
	vec3 outColor = diffuseColor*(1.0f/(distance*distance));
	return outColor;
}