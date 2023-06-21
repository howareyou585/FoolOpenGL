#ifndef MARCO_H
#define MARCO_H
#include "../GLEW/glew.h"
#define GL_INPUT_ERROR {int error = glGetError();\
if (error != 0)\
{\
	std::cout << "error:" << error << std::endl;\
}\
}

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
#endif