#version 420 core

// shader outputs
layout (location = 0) out vec4 accum;
layout (location = 1) out float reveal;

// material color
uniform vec4 color;

float calculateWeigth(float alpha)
{
	float weight = 0.0f;
    weight = alpha*max(1e-2, 3e3*pow((1-gl_FragCoord.z),3.0));
	return weight;
}
void main()
{
	// weight function
	float weight = clamp(pow(min(1.0, color.a * 10.0) + 0.01, 3.0) * 1e8 * pow(1.0 - gl_FragCoord.z * 0.9, 3.0), 1e-2, 3e3);
	//float weight = calculateWeigth(color.a);
	// store pixel color accumulation(´æ´¢ÏñËØÑÕÉ«ÀÛ»ý)
	// blend func: GL_ONE, GL_ONE
	accum = vec4(color.rgb * color.a, color.a) * weight;
	
	// store pixel revealage threshold(´æ´¢ÏñËØÏÔÊ¾ãÐÖµ)
	// blend func: GL_ZERO, GL_ONE_MINUS_SRC_COLOR
	reveal = color.a;
}