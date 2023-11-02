#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D colorBuffer;
uniform int effectType;
void main()
{    
   vec4 color = texture(colorBuffer, TexCoords);
   if(effectType==1)
   {
		color = vec4(vec3(1.0) - color.rgb, 1.0);
   }
   else  if(effectType == 2) //灰度
   {
	    float averge = (color.r+color.g+color.b)/3;
		color = vec4(averge,averge,averge,1.0f);	
   }
   else if(effectType==3) //逼真的灰度
   {
		float averge = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
		color = vec4(averge,averge,averge,1.0f);	
   }
   else if(effectType == 4) //锐化
   {
		const float offset = 1.0 / 300.0; 
		vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right    
    );

    float kernel[9] = float[](
        -1, -1, -1,
        -1,  9, -1,
        -1, -1, -1
    );
    
    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(colorBuffer, TexCoords.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];
	color = vec4(col,1.0);

   }
   else if(effectType == 5) //模糊
   {
		const float offset = 1.0 / 300.0; 
		vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right    
    );

    float kernel[9] = float[](
    1.0 / 16, 2.0 / 16, 1.0 / 16,
    2.0 / 16, 4.0 / 16, 2.0 / 16,
    1.0 / 16, 2.0 / 16, 1.0 / 16  
);
    
    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(colorBuffer, TexCoords.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];
	color = vec4(col,1.0);
   }
   FragColor = color;
   //FragColor = vec4(1.0f,1.0f,0.0f,1.0f);
}