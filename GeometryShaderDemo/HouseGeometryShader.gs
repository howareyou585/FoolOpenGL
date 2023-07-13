#version 330 core
layout(points) in;
layout(line_strip, max_vertices = 5) out;
in vec3 vColor[]; // �ڼ�����ɫ��������Ķ���һ������
out vec3 fColor;
void main()
{
	fColor = vColor[0];
	gl_Position = gl_in[0].gl_Position + vec4(-0.2,-0.2,0.0,0.0);
	EmitVertex();
	
	gl_Position = gl_in[0].gl_Position + vec4(0.2, -0.2, 0.0, 0.0);
	EmitVertex();
	
	gl_Position = gl_in[0].gl_Position + vec4(-0.2,  0.2, 0.0, 0.0);
	EmitVertex();
	
	gl_Position = gl_in[0].gl_Position + vec4(0.2,  0.2, 0.0, 0.0);
	EmitVertex();

	gl_Position = gl_in[0].gl_Position + vec4(0.0,  0.4, 0.0, 0.0);
	EmitVertex();

	EndPrimitive();
}
