#pragma once
#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <fstream>
#include <GLEW/glew.h>
#include <glm/glm.hpp>
using namespace std;
class Mesh
{
public:
	//opengl obj;
	GLuint m_vao;
	GLuint m_vbo;
	GLuint m_ebo;
	GLuint m_diffuseTexture;

	//∂•µ„ Ù–‘
	vector<glm::vec3> m_vecPositon;
	vector<glm::vec2> m_vecTexcoord;
	vector<glm::vec3> m_vecNormal;

	//
	vector<int> m_vecIndex;
public:
	Mesh() {};
	virtual ~Mesh() {};
	void BindData();
	void BindVAO();
	void UnBindVAO();
	vector<glm::vec3>& GetPosition();
	void Draw(GLuint program);
};

