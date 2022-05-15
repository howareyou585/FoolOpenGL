#version 330
out vec4 color;
in vec2 uvCoord;
uniform sampler2D tex;
void main()
{
	//color = vec4(gl_FragCoord.z,gl_FragCoord.z,gl_FragCoord.z,1.0f); 
	color = texture(tex, uvCoord);
}