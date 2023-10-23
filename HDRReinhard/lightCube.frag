#version 330
out vec4 FragColor;
uniform vec3 ucolor;
void main()
{
	FragColor = vec4(ucolor,1.0);
}