#version 330

out vec4 color;
uniform sampler2D s_texture;
in vec2 texcoord;
void main()
{
	vec4 texColor = texture(s_texture, texcoord);
	if(texColor.a < 0.1f)
	{
		discard;
	}
	color = texColor;
	//color = vec4(1.f,0.f,0.f,1.f);
}