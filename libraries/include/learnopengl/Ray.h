#ifndef RAY_H
#define RAY_H
#include "../glm/glm.hpp"
class Ray
{
public:
	Ray()
	{
	}
	Ray(const glm::vec3& origin, const glm::vec3& direction)
	{
		Define(origin, direction);
	}
	Ray(const Ray& ray) :
		m_origin(ray.m_origin), m_direction(ray.m_direction)
	{
	}
	void Define(const glm::vec3& origin, const glm::vec3& direction)
	{
		m_origin = origin;
		m_direction = glm::normalize(direction);
	}

	Ray Transformed(const glm::mat4& transform) 
	{
		Ray retRay;
		/*glm::vec3  origin(m_origin.x, m_origin.y, m_origin.z, 1.0f);
		retRay.m_origin = transform * origin;
		retRay.m_direction = transform * Vector4(m_direction, 0.0f);*/
		return retRay;
	}

	glm::vec3& GetDirection()
	{
		return m_direction;
	}
	glm::vec3& GetOrigin()
	{
		return m_origin;
	}
private:
	glm::vec3 m_origin;
	glm::vec3 m_direction;
};
#endif