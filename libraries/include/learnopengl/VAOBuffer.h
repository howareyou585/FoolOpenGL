#include "../glm/glm.hpp"
#include "../GLEW/glew.h"
#include "model.h"
#include <vector>
using namespace std;
class VAOBuffer
{
public:
	VAOBuffer(vector<Model*>& vecModel)
	{
		if (MergeMeshes(vecModel))
		{
			BuildVAO(m_vecPositon, m_vecTexcoord, m_vecNormal, m_vecIndex);
		}
	}
	VAOBuffer(vector<glm::vec3>& vecPositon,
		vector<glm::vec2>& vecTexcoord,
		vector<glm::vec3>& vecNormal,
		vector<unsigned long long>& vecIndex)
	{
		BuildVAO(vecPositon, vecTexcoord,vecNormal,vecIndex);
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
	bool MergeMeshes(vector<Model*>& vecModel)
	{
		bool bRet = false;
		unsigned long long offset = 0;
		for (size_t i = 0; i < vecModel.size(); i++)
		{
			auto* ptrModel = vecModel[i];
			auto& vecMesh = ptrModel->meshes;
			//for (auto& mesh : vecMesh)
			for (size_t j = 0; j < vecMesh.size(); j++)
			{
				auto& mesh = vecMesh[j];
				auto& vecVertex = mesh.vertices;
				for (size_t k = 0; k < vecVertex.size(); k++)
				{
					auto& vertex = vecVertex[k];
					m_vecPositon.emplace_back(vertex.Position);
					m_vecTexcoord.emplace_back(vertex.TexCoords);
					m_vecNormal.emplace_back(vertex.Normal);
					
				}
				for (size_t m = 0; m < mesh.indices.size(); m++)
				{
					m_vecIndex.push_back(mesh.indices[m] + offset);
				}
				offset += vecVertex.size();
			}
		}
		bRet = true;
		return bRet;
	}
	bool BuildVAO(vector<glm::vec3>& vecPositon,
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
