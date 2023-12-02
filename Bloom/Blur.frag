#version 330

out vec4 FragColor;
in vec2 texCoord;
uniform sampler2D t0;
uniform bool horizontal; // 水平 /竖直
float weight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

void main()
{
	vec2 tex_offset = 1.0/textureSize(t0,0);
	vec3 result = vec3(0);
	if(horizontal)
	{
		//水平方向模糊
		for(int i = 0; i < 5; i++)
		{
			result += texture(t0,vec2(texCoord.x+tex_offset.x*i, texCoord.y)).rgb*weight[i];
			result += texture(t0,vec2(texCoord.x-tex_offset.x*i, texCoord.y)).rgb*weight[i];
		}
	}
	else
	{
		//竖直方向模糊
		for(int i = 0; i < 5; i++)
		{
			result += texture(t0,vec2(texCoord.x, texCoord.y+tex_offset.y*i)).rgb*weight[i];
			result += texture(t0,vec2(texCoord.x, texCoord.y-tex_offset.y*i)).rgb*weight[i];
		}
	}
	FragColor  = vec4(result,1.0);
}