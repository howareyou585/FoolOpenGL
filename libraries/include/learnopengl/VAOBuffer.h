#ifndef VAOBUFFER_H
#define VAOBUFFER_H
#include "../glm/glm.hpp"
#include "../GLEW/glew.h"
#include "model.h"
#include <vector>
#include "Maroc.h"
using namespace std;
enum class vertex_attribute
{
	position = 0,
	normal,
	texcoord,
	color,
	tangent,
	bitangent
};
class VAOBuffer
{
public:
	VAOBuffer()
	{}
	VAOBuffer(vector<Model*>& vecModel)
	{
		if (MergeMeshes(vecModel))
		{
			BuildVAO(m_vecPositon, m_vecTexcoord, m_vecNormal, m_vecIndex);
			SetTexture(vecModel);
		}
		
	}
	VAOBuffer(vector<glm::vec3>& vecPositon,
		vector<glm::vec2>& vecTexcoord,
		vector<glm::vec3>& vecNormal,
		vector<unsigned int>& vecIndex)
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
	vector<Texture>& GetTextures()
	{
		return m_vecTexture;
	}
	bool BuildVAO(GLfloat* ptrVertex, unsigned int length,
		GLuint* ptrIndex, unsigned int indexLength,
		vector<vertex_attribute>& vecAttrib,
		map<vertex_attribute, int>& mapAttrib2Size)
	{
		bool bRet = false;
		if (!ptrVertex || !length || !vecAttrib.size() || !mapAttrib2Size.size())
		{
			return bRet;
		}
		glGenVertexArrays(1, &m_vaoId);
		glBindVertexArray(m_vaoId);
		bRet = BulidVBO(ptrVertex, length, vecAttrib, mapAttrib2Size);
		if (!bRet)
		{
			return bRet;
		}
		if (ptrIndex && indexLength)
		{
			glGenBuffers(1, &m_eboId);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eboId);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexLength, ptrIndex, GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
		glBindVertexArray(0);
		bRet = true;
		return bRet;
	}
	bool BulidVBO(GLfloat* ptrVertex, unsigned int length,
		vector<vertex_attribute>& vecAttrib,
		map<vertex_attribute, int>& mapAttrib2Size)
	{
		bool bRet = false;
		if (!ptrVertex || !length || !vecAttrib.size() || !mapAttrib2Size.size())
		{
			return bRet;
		}
		glGenBuffers(1, &m_vboId);
		glBindBuffer(GL_ARRAY_BUFFER, m_vboId);
		glBufferData(GL_ARRAY_BUFFER, length, ptrVertex, GL_STATIC_DRAW);
		int strid = 0;
		for (auto it = mapAttrib2Size.begin(); it != mapAttrib2Size.end(); it++)
		{
			strid += it->second;
		}
		strid *= sizeof(GLfloat);
		GLuint offset = 0;
		for (auto i = 0; i < vecAttrib.size(); i++)
		{
			int num = mapAttrib2Size[vecAttrib[i]];
			glVertexAttribPointer(i, num, GL_FLOAT, GL_FALSE, strid, (GLvoid*)offset);
			glEnableVertexAttribArray(i);
			offset += num * sizeof(GLfloat);
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		bRet = true;
	}
	GLuint GetVAO() { return m_vaoId; }
	GLuint GetVBO() { return m_vboId; }
	GLuint GetEBO() { return m_eboId; }
private:
	bool MergeMeshes(vector<Model*>& vecModel, vector<Vertex>& vecMergedVertex)
	{
		bool bRet = false;
		unsigned long long offset = 0;
		for (size_t i = 0; i < vecModel.size(); i++)
		{
			auto* ptrModel = vecModel[i];
			auto& vecMesh = ptrModel->meshes;
			for (size_t j = 0; j < vecMesh.size(); j++)
			{
				auto& mesh = vecMesh[j];
				auto& vecVertex = mesh.vertices;
				vecMergedVertex.insert(vecMergedVertex.end(), vecVertex.begin(), vecVertex.end());
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
	bool MergeMeshes(vector<Model*>& vecModel)
	{
		bool bRet = false;
		unsigned int offset = 0;
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
	bool BuildVAO(vector<Vertex>&vecVertex, vector<unsigned int>& vecIndex)
	{
		bool bRet = false;
		GL_INPUT_ERROR
		glGenVertexArrays(1, &m_vaoId);
		glBindVertexArray(m_vaoId);

		//使用一个VBO保存位置 法向 纹理坐标
		glGenBuffers(1, &m_vboId);
		glBindBuffer(GL_ARRAY_BUFFER, m_vboId);
		auto length = vecVertex.size()*sizeof(Vertex);
		glBufferData(GL_ARRAY_BUFFER, length, vecVertex.data(), GL_STATIC_DRAW);
		// set the vertex attribute pointers
		// vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
		

		glGenBuffers(1, &m_eboId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eboId);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, vecIndex.size() * sizeof(GLuint), vecIndex.data(), GL_STATIC_DRAW);
		GL_INPUT_ERROR
		glBindVertexArray(0);
		return true;
	}
	bool BuildVAO(vector<glm::vec3>& vecPositon,
		vector<glm::vec2>& vecTexcoord,
		vector<glm::vec3>& vecNormal,
		vector<unsigned int>& vecIndex)
	{
		GL_INPUT_ERROR
		glGenVertexArrays(1, &m_vaoId);
		glBindVertexArray(m_vaoId);
		
		//使用一个VBO保存位置 法向 纹理坐标
		glGenBuffers(1, &m_vboId);
		glBindBuffer(GL_ARRAY_BUFFER, m_vboId);
		auto length = vecPositon.size() * sizeof(glm::vec3) +
			vecNormal.size() * sizeof(glm::vec3) +
			vecTexcoord.size() * sizeof(glm::vec2);
		glBufferData(GL_ARRAY_BUFFER, length, nullptr, GL_STATIC_DRAW);
		GL_INPUT_ERROR
		//position
		GLuint offset_positon = 0;
		GLuint size_position = vecPositon.size() * sizeof(glm::vec3);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size_position, vecPositon.data());
		GL_INPUT_ERROR
		////normal
		GLuint offset_normal = size_position;
		GLuint size_normal = vecNormal.size() * sizeof(glm::vec3);
		glBufferSubData(GL_ARRAY_BUFFER, offset_normal, size_normal, vecNormal.data());
		GL_INPUT_ERROR
		////textcoord
		GLuint offset_textcoord = size_position + size_normal;
		GLuint size_textcoord = vecTexcoord.size() * sizeof(glm::vec2);
		glBufferSubData(GL_ARRAY_BUFFER, offset_textcoord, size_textcoord, vecTexcoord.data());
		GL_INPUT_ERROR
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)offset_normal);

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)offset_textcoord);
		GL_INPUT_ERROR

		glGenBuffers(1, &m_eboId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eboId);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, vecIndex.size() * sizeof(GLuint), vecIndex.data(), GL_STATIC_DRAW);
		GL_INPUT_ERROR
		glBindVertexArray(0);
		return true;
	}

	

	void SetTexture(vector<Model*>& vecModel)
	{
		for (size_t i = 0; i < vecModel.size(); i++)
		{
			auto ptrModel = vecModel[i];
			auto& vecMesh = ptrModel->meshes;
			for (auto& mesh : vecMesh)
			{
				m_vecTexture.insert(m_vecTexture.end(), mesh.textures.begin(), mesh.textures.end());
			}
		}
	}
private:
	GLuint m_vaoId{};
	GLuint m_vboId{};
	GLuint m_eboId{};
public:
	vector<glm::vec3> m_vecPositon;
	vector<glm::vec2> m_vecTexcoord;
	vector<glm::vec3> m_vecNormal;
	vector<unsigned int> m_vecIndex;
	vector<Texture> m_vecTexture;
};
#endif