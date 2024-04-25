#include "OcclusionCuller.h"
#include <GLEW/glew.h>
OcclusionCuller::OcclusionCuller(const vector<vec3>& vecColor, const vector<mat4>& vecMtx) :
	m_ptrBuffer(nullptr),m_width(0),m_height(0), 
	m_fbo(0),m_defFBO(0),m_colorBuffer(0), m_depthBuffer(0)
	
{
	m_vecOcclusionData.reserve(vecColor.size());
	for (auto i = 0; i < vecColor.size(); i++)
	{
		OcclusionData cm;
		cm.mdlOrignalColor = vecColor[i];
		cm.mdlMtx = vecMtx[i];
		cm.isOcclusion = false;
		m_vecOcclusionData.emplace_back(cm);
	}
	InitColor();
}

OcclusionCuller::~OcclusionCuller()
{
	DeleteFBO();
}

void OcclusionCuller::ResetOcclusionState()
{
	for (auto it = m_vecOcclusionData.begin(); it != m_vecOcclusionData.end(); it++)
	{
		it->isOcclusion = false;
	}
}

bool OcclusionCuller::Execute(int width, int height)
{
	bool bret = false;
	if (m_fbo < 1)
	{
		bret = CreateFBO(width, height);
	}
	else if (m_width != width || m_height != height)
	{
		bret = CreateFBO(width, height);
	}
	if (!bret)
	{
		return bret;
	}
	//渲染模型
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDisable(GL_MULTISAMPLE);
	bool bVaild = false;
	//add code
	glFinish();
	//glDisable(GL_DEPTH_TEST);
	//glDisable(GL_CULL_FACE);
	if (bVaild)
	{
		if (m_ptrBuffer)
		{
			free(m_ptrBuffer);
			m_ptrBuffer = nullptr;
		}
		int nTempBufferSize = width * height * 4;
		m_ptrBuffer = (unsigned char*)malloc(nTempBufferSize);
		
		glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, (GLubyte*)(m_ptrBuffer));
	}

	//glEnable(GL_MULTISAMPLE);
	glBindFramebuffer(GL_FRAMEBUFFER, m_defFBO);

	
	return true;
}

bool OcclusionCuller::Culling()
{
	if (!m_ptrBuffer )
	{
		return false;
	}
	
	//将场景中所有模型的可见像素数设置为0
	//ResetPiexlData();
	ResetOcclusionState();
	int nBytesPerPixel = m_nFBOBufferSize / (m_width * m_height);
	unsigned short pre_color_r = 0;
	unsigned short pre_color_g = 0;
	unsigned short pre_color_b = 0;
	unsigned short color_r = 0;
	unsigned short color_g = 0;
	unsigned short color_b = 0;
	size_t bufferOffset = 0;
	int nSimplePrecision = 1;//采样精度，每次获取颜色的像素间隔数量
	for (int y = 0; y < m_height; y = y + nSimplePrecision)
	{
		for (int x = 0; x < m_width; x = x + nSimplePrecision)
		{
			bufferOffset = (y * m_width + x) * nBytesPerPixel;
			color_r = (unsigned char)m_ptrBuffer[bufferOffset];
			color_g = (unsigned char)m_ptrBuffer[bufferOffset + 1];
			color_b = (unsigned char)m_ptrBuffer[bufferOffset + 2];
			//如果与上一次采样相同颜色，该颜色已经可见，跳过处理
			if (y > 0 && x > 0 && color_r == pre_color_r && color_g == pre_color_g && color_b == pre_color_b)
			{
				//size_t offset = color_r * 255 * 255 + color_g * 255 + color_b;
				//m_vecModelPixelCount[offset].nPixedCount++;
				continue;
			}
			pre_color_r = color_r;
			pre_color_g = color_g;
			pre_color_b = color_b;
			if (color_r == 255 && color_g == 255 && color_b == 255)
			{
				//跳过背景色
				continue;
			}

			//将颜色变为模型数据的索引
			size_t offset = color_r * 256 * 256 + color_g * 256 + color_b;
			m_vecOcclusionData[offset].isOcclusion = true;
		}
	}

	
	/*free(m_pFBOBuffer);
	m_pFBOBuffer = nullptr;
	m_nFBOBufferSize = m_FBOHeight = m_FBOWidth = 0;
	*/
	return false;
}

bool OcclusionCuller::InitColor()
{
	bool bret = false;
	unsigned char color_r = 0;
	unsigned char color_g = 0;
	unsigned char color_b = 0;
	for (int color_r = 0; color_r < 256; color_r++)
	{
		for (int color_g = 0; color_g < 256; color_g++)
		{
			for (int color_b = 0; color_b < 256; color_b++)
			{
				int index = color_r * 256 * 256 + color_g * 256 + color_b;
				if (index >= m_vecOcclusionData.size())
				{
					return true;
				}
				vec4 color(color_r / 255.f, color_g / 255.f, color_b / 255.f, 1.0f);
				m_vecOcclusionData[index].mdlColor = color;
			}
		}
	}
	bret = true;
	return bret;
}

bool OcclusionCuller::CreateFBO(int width, int height)
{
	bool bret = false;
	DeleteFBO();
	if (width <= 0 || height <= 0)
	{
		return bret;
	}
	
	m_height = height;
	m_width = width;
	//GLuint frameBuffer;
	glGenFramebuffers(1, &m_fbo);
	//GLint defaultFrameBuffer = 0;
	
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_defFBO);

	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	//颜色缓存
	unsigned int colorBuffer;
	glGenRenderbuffers(1, &colorBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, colorBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorBuffer);

	//深度缓存
	unsigned int depthBuffer;
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, /*GL_DEPTH24_STENCIL8*/ GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, /*GL_DEPTH_STENCIL_ATTACHMENT*/GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

	return true;
}

void OcclusionCuller::DeleteFBO()
{
	if (m_colorBuffer)
	{
		glDeleteRenderbuffers(1, &m_colorBuffer);
		m_colorBuffer = 0;
	}
	if (m_depthBuffer)
	{
		glDeleteRenderbuffers(1, &m_depthBuffer);
		m_depthBuffer = 0;
	}
	if (m_fbo)
	{
		glDeleteFramebuffers(1, &m_fbo);
		m_fbo = 0;
	}
}
