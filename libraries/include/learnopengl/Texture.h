#ifndef TEXTURE_H
#define TEXTURE_H
#include "../glm/glm.hpp"
#include "../GLEW/glew.h"
#include <vector>
using namespace std;
class TextureObject
{
public:
	void UpdateTexture(GLuint textureId, const char *path, const string &directory)
	{
		string filename = string(path);
		filename = directory + '/' + filename;
		int width, height, nrComponents;
		unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 100, 100, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	void UpdateTexture(GLuint textureId, unsigned char *data, int offsetx, int offsety, int width, int height)
	{
		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexSubImage2D(GL_TEXTURE_2D, 0, offsetx, offsety, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
//private:
	
};
#endif