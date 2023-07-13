#version 330 core
layout(points) in;
layout(triangle_strip, max_vertices = 5) out;
in vec3 vColor[]; // 在几何着色器中输入的都是一个数组
out vec3 fColor;
uniform bool snowing;
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

	//将最后一个点的颜色设置为白色
	gl_Position = gl_in[0].gl_Position + vec4(0.0,  0.4, 0.0, 0.0);
	if(snowing)
	{
		fColor = vec3(1.0,1.0,1.0);
	}
	EmitVertex();

	EndPrimitive();
}
