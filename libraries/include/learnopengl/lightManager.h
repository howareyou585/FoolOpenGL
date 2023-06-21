#ifndef LIGHT_MANAGER
#define LIGHT_MANAGER
#include "light.h"
#include <vector>
#include <map>
using namespace std;
//灯光管理类

class LightManager
{
public:
	void AddLight(const LightType etype, Light* ptrLight)
	{
		m_mapType2Lights[etype].push_back(ptrLight);
	}
	Light* CreateLight(const LightType etype, const glm::vec3& ambient, 
		const glm::vec3& diffuse, 
		const glm::vec3& spacular )
	{
		Light* ptrLight = nullptr;
		switch (etype)
		{
		case LightType::SPOT_LIGHT:
			//ptrLight = new Spotlight()
			break;
		default:
			break;
		}
		return ptrLight;
	}
	virtual ~LightManager()
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