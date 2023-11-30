#version 330

out vec4 FragColor;
in vec2 texCoord;
uniform sampler2D hdrBuffer;
uniform float exposure;
uniform bool enableHdrFlg;
void main()
{
	vec3 hdrColor = texture(hdrBuffer,texCoord).rgb;
	if(enableHdrFlg)
	{
		vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
		             
		FragColor = vec4(result,1.0f);
	}
	else
	{
		FragColor = vec4(hdrColor,1.0f);
	}
}