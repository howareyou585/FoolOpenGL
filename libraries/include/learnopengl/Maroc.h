#ifndef MARCO_H
#define MARCO_H
#include "../GLEW/glew.h"
#define GL_INPUT_ERROR {int error = glGetError();\
if (error != 0)\
{\
	std::cout << "error:" << error << std::endl;\
}\
}

#define LIGNT_AMBIENT_PARAM_NAME  "light.ambient"
#define LIGNT_DIFFUSE_PARAM_NAME  "light.diffuse"
#define LIGNT_SPACULAR_PARAM_NAME "light.spacluar"
#define LIGNT_ATTENUATION_CONSTANT_PARAM_NAME  "light.constant"
#define LIGNT_ATTENUATION_LINEAR_PARAM_NAME    "light.linear"
#define LIGNT_ATTENUATION_QUADRATIC_PARAM_NAME "light.quadratic"

#define MATERIAL_AMBIENT_PARAM_NAME "material.ambient"
#define MATERIAL_DIFFUSE_PARAM_NAME "material.diffuse"
#define MATERIAL_SPACULAR_PARAM_NAME "material.spacluar"
#define MATERIAL_SHINESS_PARAM_NAME "material.shiness"
#endif