#version 330
out vec4 outColor;
//layout(location = 0) out vec4 color0;
//layout(location = 1) out vec4 color1;
//layout(location = 2) out vec4 color2;
//layout(location = 3) out vec4 color3;
in vec2 texCoord;
uniform sampler2D tex;
void main()
{
	outColor = texture(tex,texCoord);
//	color0 = outColor;
//	color1 = vec4(outColor.r,0.f,0.f,1.0f);
//	color2 = vec4(0.f,outColor.g,0.f,1.0f);
//	color3 = vec4(0.f,0.f,outColor.b,1.0f);
}