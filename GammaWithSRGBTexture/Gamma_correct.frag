#version 330

out vec4 FragColor;

uniform sampler2D floorTexture;
uniform vec3 viewPos;
uniform bool gamma;
in VS_OUT
{
   vec2 texCoord;
   vec3 normalDir;
   vec3 fragPos;
}fs_in;
struct light
{
	vec3 position;
	vec3 color;
};
uniform light Light[4];
vec3 BlinnPhong(vec3 normal, vec3 fragPos, vec3 lightPos, vec3 lightColor)
{
	 // diffuse
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;    
    // simple attenuation
    float max_distance = 1.5;
    float distance = length(lightPos - fragPos);
    float attenuation = 1.0 / (gamma ? distance * distance : distance);
    
    diffuse *= attenuation;
    specular *= attenuation;
    
    return diffuse + specular;
}
void main()
{
	
	vec3 color = texture(floorTexture, fs_in.texCoord).rgb;
    vec3 lighting = vec3(0.0);
    for(int i = 0; i < 4; ++i)
    {   
        lighting += BlinnPhong(normalize(fs_in.normalDir), fs_in.fragPos, Light[i].position, Light[i].color);
    }
    color *= lighting;
	if(gamma)
	{
		color = pow(color, vec3(1.0/2.2));
	}
	FragColor =vec4(color,1.0f);
}