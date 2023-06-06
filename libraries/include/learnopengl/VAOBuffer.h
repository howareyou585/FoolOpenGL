#include "../glm/glm.hpp"
#include "../GLEW/glew.h"
#include <vector>
using namespace std;
class VAOBuffer
{
public:
	VAOBuffer(vector<glm::vec3>& vecPositon,
		vector<glm::vec2>& vecTexcoord,
		vector<glm::vec3>& vecNormal,
		vector<unsigned long long>& vecIndex)
	{
		glGenVertexArrays(1, &m_vaoId);
		glBindVertexArray(m_vaoId);
		//使用一个VBO保存位置 法向 纹理坐标
		glGenBuffers(1, &m_vboId);
		glBindBuffer(GL_ARRAY_BUFFER, m_vboId);
		auto length = vecPositon.size() * sizeof(glm::vec3) +
			vecNormal.size() * sizeof(glm::vec3) +
			vecTexcoord.size() * sizeof(glm::vec2);
		glBufferData(GL_ARRAY_BUFFER, vecPositon.size() * sizeof(glm::vec3), nullptr, GL_STATIC_DRAW);
		//position
		GLuint offset_positon = 0;
		GLuint size_position = vecPositon.size() * sizeof(glm::vec3);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size_position, vecPositon.data());
		////normal
		GLuint offset_normal = size_position;
		GLuint size_normal = vecNormal.size() * sizeof(glm::vec3);
		glBufferSubData(GL_ARRAY_BUFFER, offset_normal, size_normal, vecNormal.data());

		////textcoord
		GLuint offset_textcoord = size_position + size_normal;
		GLuint size_textcoord = vecTexcoord.size() * sizeof(glm::vec2);
		glBufferSubData(GL_ARRAY_BUFFER, offset_textcoord, size_textcoord, vecTexcoord.data());

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)offset_normal);

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)offset_textcoord);


		glGenBuffers(1, &m_eboId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eboId);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, vecIndex.size() * sizeof(GLuint), vecIndex.data(), GL_STATIC_DRAW);
		glBindVertexArray(0);
		m_vecPositon.swap(vecPositon);
		m_vecTexcoord.swap(vecTexcoord);
		m_vecIndex.swap(vecIndex);
		m_vecNormal.swap(vecNormal);
	}
	void Bind()
	{
		glBindVertexArray(m_vaoId);
	}
	void UnBind()
	{
		glBindVertexArray(0);
	}
	void BindEBO()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eboId);
	}
	void UnBindEBO()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
private:
	GLuint m_vaoId;
	GLuint m_vboId;
	GLuint m_eboId;
public:
	vector<glm::vec3> m_vecPositon;
	vector<glm::vec2> m_vecTexcoord;
	vector<glm::vec3> m_vecNormal;
	vector<unsigned long long> m_vecIndex;
};
