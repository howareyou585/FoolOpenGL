#version 330 core
layout(points) in;
layout(points, max_vertices = 1) out;
in vec3 vColor[]; // 在几何着色器中输入的都是一个数组
out vec3 fColor;
void main()
{
	fColor = vColor[0];
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();
	EndPrimitive();
}
