#ifndef VIEWPORT_H
#define VIEWPORT_H
#include "../glm/glm.hpp"
#include "camera.h"
#include "Ray.h"
#include "Rect.h"
class Viewport
{
public:
	Viewport(Camera* ptrCamera, const int x, const int y, const int width, const int height)
	{
		m_ptrCamera = ptrCamera;
		m_x = x;
		m_y = y;
		m_width = width;
		m_height = height;
	}
	virtual ~Viewport()
	{
		
	}
	Ray GetScreenRay(int x, int y)
	{
		if (!m_ptrCamera)
		{
			return Ray();
		}
		
		float screenX = 0.f;
		float screenY = 0.f;

		/*if (m_rect != IntRect::ZERO)
		{*/
		//将屏幕坐标变换到[0,1]
		screenX = float(x - m_rect.m_left) / (float)m_rect.Width();
		screenY = float(y - m_rect.m_top) / (float)m_rect.Height();
		/*}*/
		
	}
private:
	Camera* m_ptrCamera{};
	int m_x{};
	int m_y{};
	int m_height{};
	int m_width{};
};
#endif