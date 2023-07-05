#ifndef RECT_H
#define RECT_H
#include "../glm/glm.hpp"
#include "EnumDefine.h"
class Rect
{
public:
	Rect()
	{

	}
	~Rect()
	{

	}
private:
	/**
	* @brief Minimum vector.
	*/
	glm::vec3 m_min;
	/**
	* @brief Maximum vector.
	*/
	glm::vec3 m_max;
	/**
	* @brief Defined flag.
	*/
	bool m_defined{};
};

class IntRect
{
public:
	
	IntRect() :
		m_left(0), m_top(0), m_right(0), m_bottom(0)
	{
	}

	IntRect(int left, int top, int right, int bottom) :
		m_left(left), m_top(top), m_right(right), m_bottom(bottom)
	{
	}

	bool operator ==(const IntRect& rhs) const
	{
		return m_left == rhs.m_left && m_top == rhs.m_top
			&& m_right == rhs.m_right && m_bottom == rhs.m_bottom;
	}
	
	bool operator !=(const IntRect& rhs) const
	{
		return m_left != rhs.m_left || m_top != rhs.m_top
			|| m_right != rhs.m_right || m_bottom != rhs.m_bottom;
	}

	glm::ivec2 Size() 
	{
		return IntVector2(Width(), Height());
	}

	glm::ivec2 Center() ;

	int Width() 
	{
		return m_right - m_left;
	}
	
	int Height() 
	{
		return m_bottom - m_top;
	}

	IntersectionState IsInside(const IntVector2& point) const
	{
		if (point.m_x < m_left || point.m_y < m_top || point.m_x >= m_right
			|| point.m_y >= m_bottom)
			return IntersectionState::OUTSIDE;
		else
			return IntersectionState::INSIDE;
	}
private:
	int m_left{};
	int m_top{};
	int m_right{};
	int m_bottom{};
};

#endif