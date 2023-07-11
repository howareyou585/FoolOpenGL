#version 330 core
layout(points) in;
layout(triangle_strip, max_vertices = 5) in out;
in vec3 color;
out vec3 fcolor;
void build_house(vec4 pos);

void main()
{
	build_house(gl_in[0].gl_position);
}
void build_house(vec4 pos)
{
	
}