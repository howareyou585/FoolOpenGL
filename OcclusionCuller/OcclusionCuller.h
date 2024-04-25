#pragma once
#include <vector>
#include <map>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
using namespace std;
using namespace glm;
typedef struct tagOcclusionData
{
	vec3 mdlOrignalColor;
	vec4 mdlColor;
	mat4 mdlMtx; 
	bool isOcclusion;
}OcclusionData;
class OcclusionCuller
{
public:
	OcclusionCuller(const vector<vec3>& vecColor, const vector<mat4>& vecMtx);
	~OcclusionCuller();
	OcclusionCuller(const  OcclusionCuller&) = delete;
	OcclusionCuller& operator=(const OcclusionCuller&) = delete;
	void ResetOcclusionState();
	bool Execute(int width, int height);
	bool Culling();
private :
	bool InitColor();
	bool CreateFBO(int width, int height);
	void DeleteFBO();
	
private:
	vector<OcclusionData> m_vecOcclusionData;
	int m_width;
	int m_height;
	unsigned char* m_ptrBuffer;
	unsigned int m_fbo;
	int m_defFBO;
	unsigned int m_colorBuffer;
	unsigned int m_depthBuffer;
	size_t m_nFBOBufferSize;					//!<FBOÄÚÈÝ»º´æ´óÐ¡
	
};

