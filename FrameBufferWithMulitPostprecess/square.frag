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
   else  if(effectType == 2)
   {
	    float averge = (color.r+color.g+color.b)/3;
		color = vec4(averge,averge,averge,1.0f);	
   }
   FragColor = color;
   //FragColor = vec4(1.0f,1.0f,0.0f,1.0f);
}