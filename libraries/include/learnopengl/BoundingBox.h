#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include "../glm/glm.hpp"
class BoundingBox
{
public:
	glm::vec3 GetCenter()
	{
		glm::vec3 center = (m_maxPnt + m_minPnt) * 0.5f;
		return center;
	}
	float GetLength()
	{
		float distance = glm::distance(m_maxPnt, m_minPnt);
		return distance;
	}
public:
	glm::vec3 m_minPnt;
	glm::vec3 m_maxPnt;
};

#endif