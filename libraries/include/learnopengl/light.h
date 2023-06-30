#ifndef LIGHT_H
#define LIGHT_H
#include "../glm/glm.hpp"
#include "shader.h"
#include "Maroc.h"
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
	virtual bool SetLightUniformParam(Shader& shader, const string& strParaName)
	{
		bool bRet = false;
		string str;
		str = strParaName +  AMBIENT_PARAM_NAME;
		shader.setVec3(str, m_ambient);
		GL_INPUT_ERROR
		str = strParaName  + DIFFUSE_PARAM_NAME;
		shader.setVec3(str, m_diffuse);
		GL_INPUT_ERROR
		str = strParaName  + SPACULAR_PARAM_NAME;
		shader.setVec3(str, m_spacular);
		GL_INPUT_ERROR
		bRet = true;
		return bRet;
	}
protected:
	void SetAttenuationUniformParam(Shader& shader, const string& strParaName)
	{
		string str;
		str = strParaName + ATTENUATION_CONSTANT_PARAM_NAME;
		shader.setFloat(str, m_attenuatedConstant);
		str = strParaName + ATTENUATION_LINEAR_PARAM_NAME;
		shader.setFloat(str, m_attenuatedLinear);
		str = strParaName + ATTENUATION_QUADRATIC_PARAM_NAME;
		shader.setFloat(str, m_attenuatedQuadratic);
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
{
public:
	Directionlight(glm::vec3& ambient, glm::vec3& diffuse,glm::vec3& spacluar,glm::vec3& direction) :Light(ambient, diffuse, spacluar)
	{
		
		m_direction = direction;
	
	}
	virtual ~Directionlight()
	{

	}
	virtual bool SetLightUniformParam(Shader& shader, const string& strParaName)
	{
		Light::SetLightUniformParam(shader, strParaName);
		string str = strParaName + DIRECTION_PARAM_NAME;
		shader.setVec3(str, m_direction);
		return true;
	}
private:
	glm::vec3 m_direction;
};
//点光
class PointLight :public Light
{
public:
	PointLight(glm::vec3& ambient, glm::vec3& diffuse, glm::vec3& spacluar, glm::vec3& position) :Light(ambient, diffuse, spacluar)
	{
		m_position = position;
	}
	virtual ~PointLight()
	{

	}
	virtual bool SetLightUniformParam(Shader& shader, const string& strParaName)
	{
		Light::SetLightUniformParam(shader, strParaName);
		string str;
		str = strParaName + POSITION_PARAM_NAME;
		SetAttenuationUniformParam(shader, strParaName);
		return true;

	}
	void SetPosition(glm::vec3 & position)
	{
		m_position = position;
	}
private:
	glm::vec3 m_position;
};
//聚光
class Spotlight :public Light
{
public:
	Spotlight(glm::vec3 & ambient, glm::vec3 & diffuse,
		glm::vec3 & spacluar, glm::vec3 & postion, glm::vec3& direction, float cutoffInnerAngle=0.0f, float cutOffOuterAngle=0.0f):Light(ambient, diffuse, spacluar)
	{
		m_cutoffInnerAngle = cutoffInnerAngle;
		m_cutoffOuterAngle = cutOffOuterAngle;
		m_position = postion;
		m_spotDir = direction;
		m_spotDir = glm::normalize(m_spotDir);
	}
	virtual ~Spotlight()
	{

	}
	virtual bool SetLightUniformParam(Shader& shader, const string& strParaName)
	{
		Light::SetLightUniformParam(shader, strParaName);
		string str;
		str = strParaName + POSITION_PARAM_NAME;
		shader.setVec3(str, m_position);
		GL_INPUT_ERROR
		str = strParaName + DIRECTION_PARAM_NAME;
		shader.setVec3(str, m_spotDir);
		GL_INPUT_ERROR
		str = strParaName + SPOT_LIGHT_CUTOFF_PARAM_NAME;
		shader.setFloat(str, m_cutoffInnerAngle);
		GL_INPUT_ERROR
		str = strParaName + SPOT_LIGHT_OUTER_CUTOFF_PARAM_NAME;
		shader.setFloat(str, m_cutoffOuterAngle);
		GL_INPUT_ERROR
		SetAttenuationUniformParam(shader, strParaName);
		return true;
	
	}
	void SetCutoffInnerAngle(float angle)
	{
		m_cutoffInnerAngle = angle;
	}
	void SetCutoffOuterAngle(float angle)
	{
		m_cutoffOuterAngle = angle;
	}
	float GetCutoffInnerAngle()
	{
		return m_cutoffInnerAngle;
	}
	float GetCutoffOuterAngle()
	{
		return m_cutoffOuterAngle;
	}
private:
	glm::vec3 m_position; //聚光灯的位置
	glm::vec3 m_spotDir; //聚光的方向
	float m_cutoffInnerAngle{}; //内切光角
	float m_cutoffOuterAngle{}; //外切光角
};
//半球光
class HemisphereLight:Light
{
public:
	HemisphereLight(glm::vec3& topColor, glm::vec3& groundColor, glm::vec3& direction) :Light()
	{
		m_topColor = topColor;
		m_groundColor = groundColor;
		m_direction = direction;
	}
	virtual bool SetLightUniformParam(Shader& shader, const string& strParaName)
	{
		string str;
		str = strParaName + DIRECTION_PARAM_NAME;
		shader.setVec3(str, m_direction);
		str = strParaName + HEMISPHERE_SKY_COLOR;
		shader.setVec3(str, m_topColor);
		str = strParaName + HEMISPHERE_GROUND_COLOR;
		shader.setVec3(str, m_groundColor);
		return true;

	}
private:
	glm::vec3 m_topColor;
	glm::vec3 m_groundColor;
	glm::vec3 m_direction;
};
#endif