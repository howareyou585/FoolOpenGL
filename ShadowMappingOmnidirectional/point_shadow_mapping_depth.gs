#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

uniform mat4 shadowMatrices[6];

out vec4 FragPos;// FragPos from GS (output per emitvertex)
void main()
{
	for(int face = 0; face < 6 ; face ++)
	{
		gl_Layer = face;// built-in variable that specifies to which face we render.
		for(int i = 0; i < 3; i++)
		{
			FragPos = gl_in[i].gl_Position; // 世界坐标下的顶点
            gl_Position = shadowMatrices[face] * FragPos;//灯光空间下 的顶点
            EmitVertex();
		}
		EndPrimitive();
	}
}