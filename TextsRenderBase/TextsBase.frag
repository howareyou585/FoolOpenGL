#version 330

out vec4 color;
uniform sampler2D s_texture; //一个是单颜色的字形位图纹理
uniform vec3 textColor;//颜色纹理
in vec2 texcoord;
void main()
{
	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(s_texture, texcoord).r*0.6);
	color = vec4(textColor,1.0)*sampled;
}