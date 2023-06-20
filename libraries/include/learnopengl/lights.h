#ifndef LIGHTS_H
#define LIGHTS_H
#include "light.h"
#include <vector>
#include <map>
using namespace std;
//灯光管理类

class Lights
{
public:
	void AddLight(const LightType etype, Light* ptrLight)
	{
		m_mapType2Lights[etype].push_back(ptrLight);
	}
	virtual ~Lights()
	{
		for (auto it = m_mapType2Lights.begin(); it != m_mapType2Lights.end(); it++)
		{
			auto & vecLight = it->second;
			for (auto ptrLight : vecLight)
			{
				if (ptrLight)
				{
					delete ptrLight;
					ptrLight = nullptr;
				}
			}
		}
		m_mapType2Lights.clear();
	}
private:
	map< LightType, vector<Light*>> m_mapType2Lights;
	
};

#endif