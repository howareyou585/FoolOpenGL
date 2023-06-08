#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include "../glm/glm.hpp"
class BoundingBox
{
public:
	BoundingBox()
	{}
	BoundingBox(glm::vec3& minPnt, glm::vec3& maxPnt)
	{
		m_minPnt = minPnt;
		m_maxPnt = maxPnt;
	}
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
	void Merge(const glm::vec3& point)
	{
		if (point.x < m_minPnt.x)
			m_minPnt.x = point.x;
		if (point.y < m_minPnt.y)
			m_minPnt.y = point.y;
		if (point.z < m_minPnt.z)
			m_minPnt.z = point.z;
		if (point.x > m_maxPnt.x)
			m_maxPnt.x = point.x;
		if (point.y > m_maxPnt.y)
			m_maxPnt.y = point.y;
		if (point.z > m_maxPnt.z)
			m_maxPnt.z = point.z;
	}

	/**
	* @brief Merge another bounding box.
	*
	* @param box
	*/
	void Merge(const BoundingBox& box)
	{
		if (box.m_minPnt.x < m_minPnt.x)
			m_minPnt.x = box.m_minPnt.x;
		if (box.m_minPnt.y < m_minPnt.y)
			m_minPnt.y = box.m_minPnt.y;
		if (box.m_minPnt.z < m_minPnt.z)
			m_minPnt.z = box.m_minPnt.z;
		if (box.m_maxPnt.x > m_maxPnt.x)
			m_maxPnt.x = box.m_maxPnt.x;
		if (box.m_maxPnt.y > m_maxPnt.y)
			m_maxPnt.y = box.m_maxPnt.y;
		if (box.m_maxPnt.z > m_maxPnt.z)
			m_maxPnt.z = box.m_maxPnt.z;
	}
	BoundingBox BoundingBox::Transformed(const glm::mat4& transform)
	{
		
		glm::vec4 minPnt4(m_minPnt.x, m_minPnt.y, m_minPnt.z, 1.0f);
		glm::vec4 maxPnt4(m_maxPnt.x, m_maxPnt.y, m_maxPnt.z, 1.0f);
		minPnt4 = minPnt4 * transform;
		maxPnt4 = maxPnt4 * transform;
		glm::vec3 minPnt(minPnt4.x, minPnt4.y, minPnt4.z);
		glm::vec3 maxPnt(maxPnt4.x, maxPnt4.y, maxPnt4.z);
		return BoundingBox(minPnt, maxPnt);
	}
	/**
			* @brief Return size.
			*
			* @return
			*/
	glm::vec3 Size()
	{
		return m_maxPnt - m_minPnt;
	}
public:
	glm::vec3 m_minPnt;
	glm::vec3 m_maxPnt;
};

#endif