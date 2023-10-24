#version 330

out vec4 FragColor;
in vec2 texCoord;
uniform sampler2D s_texture;
uniform sampler2D s_texture2;
uniform bool gammaState ;
void main()
{
	float gamma = 2.2;
	
	vec3 color  = mix(texture(s_texture, texCoord),texture(s_texture2, texCoord),0.2).rgb;
	if(gammaState)
	{
		color = pow(color, vec3(1.0/gamma));
	}
	FragColor =vec4( color,1.0f);
}