#pragma once
#include <glm/glm.hpp>
struct Material
{
    glm::vec4 ambient;//灯的环境光
    glm::vec4 diffuse;//灯的漫反射光
    glm::vec4 specular;//灯的镜面反射光
}; 

