#version 330 core

out vec4 FragColor;
in vec2 texCoord;
uniform sampler2D s_texture;
uniform sampler2D s_texture2;
float near = 0.1; 
float far = 100.0; 
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}
void main()
{
	
	FragColor = mix(texture(s_texture, texCoord),texture(s_texture2, texCoord),0.2);
	
	// float depth = LinearizeDepth(gl_FragCoord.z) / far; // divide by far to get depth in range [0,1] for visualization purposes
    //FragColor = vec4(vec3(depth), 1.0);
}