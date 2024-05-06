#ifndef MARCO_H
#define MARCO_H
#include "../GLEW/glew.h"
#include <string>
#include <iostream>
using namespace std;
GLenum glCheckError_(const char* file, int line)
{
	GLenum errorcode = 0;
	do 
	{
		errorcode = glGetError();
		if (!errorcode)
		{
			break;
		}
		string strErr;
		switch (errorcode)
		{
		case GL_INVALID_ENUM:                  strErr = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 strErr = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             strErr = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                strErr = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               strErr = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 strErr = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: strErr = "INVALID_FRAMEBUFFER_OPERATION"; break;
		default:
			strErr = "Unknow error";
		}
		std::cout << strErr << " | " << file << " (" << line << ")" << std::endl;
	} while (0);
	
	return errorcode;
}

#define GL_CHECK_ERROR glCheckError_(__FILE__, __LINE__)
#define GL_INPUT_ERROR do {int error = glGetError();\
if (error != 0)\
{\
	std::cout << "error:" << error << std::endl;\
}\
}while(0);




#define AMBIENT_PARAM_NAME  "ambient"
#define DIFFUSE_PARAM_NAME  "diffuse"
#define SPACULAR_PARAM_NAME "spacluar"
#define ATTENUATION_CONSTANT_PARAM_NAME  "constant"
#define ATTENUATION_LINEAR_PARAM_NAME    "linear"
#define ATTENUATION_QUADRATIC_PARAM_NAME "quadratic"
#define SHINESS_PARAM_NAME "material.shiness"

#define POSITION_PARAM_NAME "position"
#define DIRECTION_PARAM_NAME "direction"
#define SPOT_LIGHT_CUTOFF_PARAM_NAME "cutoff"
#define SPOT_LIGHT_OUTER_CUTOFF_PARAM_NAME "outerCutoff"

#define HEMISPHERE_SKY_COLOR "skyColor"
#define HEMISPHERE_GROUND_COLOR "groundColor"
#endif