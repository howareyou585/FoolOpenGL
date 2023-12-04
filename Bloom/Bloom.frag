#version 330 core

out vec4 FragColor;
in vec2 texCoord;
uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform float exposure;

void main()
{
	vec3 hdrColor = texture(scene,texCoord).rgb;
	vec3 bloomColor = texture(scene,texCoord).rgb;
	hdrColor += bloomColor;
	vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
	FragColor = vec4(result,1.0);
}