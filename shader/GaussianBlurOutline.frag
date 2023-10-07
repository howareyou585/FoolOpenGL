#version 330 core
uniform float outlineWidth;
uniform sampler2D height;
uniform sampler2D model;
out  vec2 texCoords;
void main()
{
	vec4 gaussianColor = texture2D(height, texCoords);
	vec4 modelColor = texture2D(model, texCoords);
	vec4 final = gaussianColor - modelColor;
	final = clamp(final*outlineWidth,0.0,1.0);
	gl_FragColor = final;
};