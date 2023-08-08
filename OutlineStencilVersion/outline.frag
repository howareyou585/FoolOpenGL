#version 330
out vec4 color;
in vec2 uvCoord;
uniform sampler2D tex;
void main()
{
	//color = vec4(gl_FragCoord.z,gl_FragCoord.z,gl_FragCoord.z,1.0f); 
	color = vec4(0.04, 0.28, 0.26, 1.0);
}