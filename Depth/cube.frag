#version 330
out vec4 color;
void main()
{
	
	color = texture(tex, uvCoord);
}