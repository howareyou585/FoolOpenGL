#version 330 core
out vec4 FragColor;
in VERTEX_INFO
{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
}frag_info;

struct Light
{
	vec3 Position;
	vec3 Color;
};
uniform Light lights[16];
//uniform vec3 viewPos;
uniform sampler2D diffuseTexture;
void main()
{
	vec3 color = texture(diffuseTexture, frag_info.TexCoords).rgb;
    vec3 normal = normalize(frag_info.Normal);
    // ambient
    vec3 ambient = 0.0 * color;
    // lighting
    vec3 lighting = vec3(0.0);
    for(int i = 0; i < 16; i++)
    {
        // diffuse
        vec3 lightDir = normalize(lights[i].Position - frag_info.FragPos);
        float diff = max(dot(lightDir, normal), 0.0);
        vec3 diffuse = lights[i].Color * diff * color;      
        vec3 result = diffuse;        
        // attenuation (use quadratic as we have gamma correction)
        float distance = length(frag_info.FragPos - lights[i].Position);
        result *= 1.0 / (distance * distance);
        lighting += result;
                
    }
    FragColor = vec4(ambient + lighting, 1.0);
}