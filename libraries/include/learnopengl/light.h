#ifndef LIGHT_H
#define LIGHT_H
#include "../glm/glm.hpp"
enum class LightType
{
	DIRECTION_LIGHT = 0,
	POINT_LIGHT,
	SPOT_LIGHT
};
//灯光基类
class Light
{
public:
	Light() :m_ambient(0.2f, 0.2f, 0.2f),
		m_diffuse(0.6f, 0.6f, 0.6f),
		m_spacular(1.0f, 1.0f, 1.0f)
	{

	}
	Light(glm::vec3 & ambient, glm::vec3 & diffuse,
		glm::vec3 & spacluar)
	{
		m_ambient = ambient;
		m_diffuse = diffuse;
		m_spacular = spacluar;
	}
	virtual ~Light()
	{}
	void SetAmbient(glm::vec3 & ambient)
	{
		m_ambient = ambient;
	}
	void SetDiffuse(glm::vec3 & diffuse)
	{
		m_diffuse = diffuse;
	}
	void SetSpacular(glm::vec3 & spacular)
	{
		m_spacular = spacular;
	}
	void SetAttenuatedConstant(float val)
	{
		m_attenuatedConstant = val;
	}
	void SetAttenuatedLinear(float val)
	{
		m_attenuatedLinear = val;
	}
	void SetAttenuatedQuadratic(float val)
	{
		m_attenuatedQuadratic = val;
	}
protected:
	glm::vec3 m_ambient;
	glm::vec3 m_diffuse;
	glm::vec3 m_spacular;
	float m_attenuatedConstant{ 1.0f };
	float m_attenuatedLinear;
	float m_attenuatedQuadratic;
};
//平行光
class Directionlight :public Light
{};
//点光
class PointLight :public Light
{

};
//聚光
class Spotlight :public Light
{
public:
	Spotlight(glm::vec3 & ambient, glm::vec3 & diffuse,
		glm::vec3 & spacluar, glm::vec3 & postion, float cutoffAngle):Light(ambient, diffuse, spacluar)
	{
	}
private:
	glm::vec3 m_position;
	float m_cutoffAngle;
};
//半球光....
#endif