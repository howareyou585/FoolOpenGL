#version 330 core
layout(points) in;
layout(points, max_vertices = 1) out;
in vec3 vColor[]; // �ڼ�����ɫ��������Ķ���һ������
out vec3 fColor;
void main()
{
	fColor = vColor[0];
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();
	EndPrimitive();
}
