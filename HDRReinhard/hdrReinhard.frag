#version 330

out vec4 FragColor;
in vec2 texCoord;
uniform sampler2D colorBuffer;

void main()
{
	
	vec3 color = texture(colorBuffer, texCoord).rgb;
	color = color/((vec3(1.0)+color));
	FragColor = vec4(color,1.0);
}