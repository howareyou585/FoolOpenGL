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
	FBOBuffer(map<GLenum, AttachmentType>& mapColorAttachment, bool bAttachedDepth, bool bAttachedStencil):
		m_bAttachedDepth(bAttachedDepth),
		m_bAttachedStencil(bAttachedStencil)
	{
		m_mapColorAttachment = mapColorAttachment;
	}
	
	bool Create()
	{
		bool bRet = false;
		glGenFramebuffers(1, &m_fboId);
		glBindFramebuffer(GL_FRAMEBUFFER, m_fboId);
		GLuint gPostion = 0;
		GLuint gNormal = 0;
		GLuint gAlbedoSpace = 0;
		//todo
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
};
#endif