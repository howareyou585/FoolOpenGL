#version 330

out vec4 color;
in vec3 TextCoord;
uniform samplerCube skybox;
void main()
{
	color = texture(skybox, TextCoord);
}