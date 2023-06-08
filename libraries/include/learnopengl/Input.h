#ifndef INPUT_H
#define INPUT_H
#include "camera.h"
class Input
{
public:
	Input(Camera* ptrCamera, float lastX, float lastY, float deltaTime = 0.f, float lastFrame=0.f) :m_firstMouse{true}
	{
		m_ptrCamera = ptrCamera;
		m_lastX = lastX;
		m_lastY = lastY;
		m_deltaTime = deltaTime;
		m_lastFrame = lastFrame;
	}
	Camera* GetCamera()
	{
		return m_ptrCamera;
	}
private:
	Camera* m_ptrCamera{};
	float m_lastX;
	float m_lastY;
	bool m_firstMouse;

	// timing
	float m_deltaTime{};
	float m_lastFrame{};

};
#endif
