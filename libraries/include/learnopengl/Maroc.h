#ifndef MARCO_H
#define MARCO_H
#include "../GLEW/glew.h"
#define GL_INPUT_ERROR {int error = glGetError();\
if (error != 0)\
{\
	std::cout << "error:" << error << std::endl;\
}\
}
#endif