#ifndef FBOBUFFER_H
#define FBOBUFFER_H
#include "../glm/glm.hpp"
#include "../GLEW/glew.h"
#include <vector>
#include <map>
using namespace std;
enum class AttachmentType
{
	Texture,
	Depth
};
class FBOBuffer
{
public:
	FBOBuffer(int width, int height,
		map<GLenum, AttachmentType>& mapColorAttachment, bool bAttachedDepth, bool bAttachedStencil):
		m_width(width),
		m_height(height),
		m_bAttachedDepth(bAttachedDepth),
		m_bAttachedStencil(bAttachedStencil)
	{
		m_mapColorAttachment = mapColorAttachment;
	}
	void SetSize(int width, int height)
	{
		m_width = width;
		m_height = height;
	}
	bool Create()
	{
		bool bRet = false;
		
		glGenFramebuffers(1, &m_fboId);
		glBindFramebuffer(GL_FRAMEBUFFER, m_fboId);
		vector<GLenum> vecAttachment;
		vecAttachment.reserve(m_mapColorAttachment.size());
		for (auto it = m_mapColorAttachment.begin(); it != m_mapColorAttachment.end(); it++)
		{
			GLuint textureId{};
			glGenTextures(1, &textureId);
			glBindTexture(GL_TEXTURE_2D, textureId);
			//glTexImage2D(GL_TEXTURE_2D,0, GL_RGBA,)
		/*	glFramebufferTexture2D()
			glBindTexture(GL_TEXTURE_2D, 0);*/
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return bRet;
	}

	void Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_fboId);
	}
	void UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER,0);
	}
	GLuint GetFBOId()
	{
		return m_fboId;
	}
private:
	GLuint m_fboId{};
	map<GLenum, AttachmentType> m_mapColorAttachment;
	bool m_bAttachedDepth{};
	bool m_bAttachedStencil{};
	int  m_width{};
	int  m_height{};
};
#endif