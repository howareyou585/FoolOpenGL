#ifndef _SKY_BOX_INCLUDE_H
#define _SKY_BOX_INCLUDE_H
#include <string>
#include <vector>
#include <stb_image.h>
#include "../GLEW/glew.h"
using namespace std;

class SkyBox
{
public:
	SkyBox(const vector<string>& vecFile)
	{
		m_vecFile.swap(vecFile);
	}
	
	virtual ~SkyBox();
	unsigned int LoadCubeMap()
	{
		unsigned int textureId;
		if (m_vecFile.size() < 6)
		{
			return -1;
		}
		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
		for (int i = 0; i < m_vecFile.size(); i++)
		{
			if (i == 6)
			{
				break;
			}
			int  nrComponents;
			int  width = 0; height = 0;
			unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				stbi_image_free(data);
			}
			else
			{
				return -1;
			}
		}
		
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		m_textureId = textureId;
		return textureId;
	}
public:
	// 指定包围盒的顶点属性 位置
	GLfloat m_vertices[] = {
		// 背面
		-1.0f, 1.0f, -1.0f,		// A
		-1.0f, -1.0f, -1.0f,	// B
		1.0f, -1.0f, -1.0f,		// C
		1.0f, -1.0f, -1.0f,		// C
		1.0f, 1.0f, -1.0f,		// D
		-1.0f, 1.0f, -1.0f,		// A

		// 左侧面
		-1.0f, -1.0f, 1.0f,		// E
		-1.0f, -1.0f, -1.0f,	// B
		-1.0f, 1.0f, -1.0f,		// A
		-1.0f, 1.0f, -1.0f,		// A
		-1.0f, 1.0f, 1.0f,		// F
		-1.0f, -1.0f, 1.0f,		// E

		// 右侧面
		1.0f, -1.0f, -1.0f,		// C
		1.0f, -1.0f, 1.0f,		// G
		1.0f, 1.0f, 1.0f,		// H
		1.0f, 1.0f, 1.0f,		// H
		1.0f, 1.0f, -1.0f,		// D
		1.0f, -1.0f, -1.0f,		// C

		// 正面
		-1.0f, -1.0f, 1.0f,  // E
		-1.0f, 1.0f, 1.0f,  // F
		1.0f, 1.0f, 1.0f,  // H
		1.0f, 1.0f, 1.0f,  // H
		1.0f, -1.0f, 1.0f,  // G
		-1.0f, -1.0f, 1.0f,  // E

		// 顶面
		-1.0f, 1.0f, -1.0f,  // A
		1.0f, 1.0f, -1.0f,  // D
		1.0f, 1.0f, 1.0f,  // H
		1.0f, 1.0f, 1.0f,  // H
		-1.0f, 1.0f, 1.0f,  // F
		-1.0f, 1.0f, -1.0f,  // A

		// 底面
		-1.0f, -1.0f, -1.0f,  // B
		-1.0f, -1.0f, 1.0f,   // E
		1.0f, -1.0f, 1.0f,    // G
		1.0f, -1.0f, 1.0f,    // G
		1.0f, -1.0f, -1.0f,   // C
		-1.0f, -1.0f, -1.0f,  // B
	};
private:
	vector<string> m_vecFile;
	unsigned int m_textureId;
};
#endif