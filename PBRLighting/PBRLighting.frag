#version 330

out vec4 color;
uniform vec3 camPos;
uniform float _roughness;
uniform float _metallic;
uniform vec3 lightPositions;
uniform vec3 lightColors;
in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;
const float PI =3.14159265359;
//���򷢲�����
//N ���߷���
//H �������
//roughness �ֲڶ�
float DistributionGGX(vec3 N, vec3 H, float roughness) 
{
	float a = roughness*roughness;
	float square_a = a*a;
	float NdotH = max(dot(N,H),0.0);
	float square_NdotH = NdotH*NdotH;

	float nom = square_a;//����
	float denom = (square_NdotH * (square_a - 1.0) + 1.0);
	denom = PI * denom * denom; //��ĸ
	return nom/denom;
}
void main()
{
	vec3 nDir = normalize(Normal) ; // ��������
	vec3 vDir = normalize(camPos- WorldPos);//
	vec3 lDir = normalize(lightPositions - WorldPos); // �ƹ�ķ���
    vec3 hDir = normalize(vDir + lDir);//�������
	float dGGX =  DistributionGGX(nDir,hDir, _roughness);
	color = vec4(dGGX,dGGX,dGGX, 1.0);
}