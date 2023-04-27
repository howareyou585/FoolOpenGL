#version 330
//uniform sampler2D tex;
out vec4 color;
//in vec2 TextCoord;
void main()
{
	
	//color = texture(tex,vec2(TextCoord.s, 1.0-TextCoord.t));
	color = vec4(1.0,0.0,0.0,1.0);
}