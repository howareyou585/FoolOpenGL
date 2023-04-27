#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <GLEW/glew.h>
#include <iostream>
using namespace Assimp;
using namespace glm; 
void Model::Load(const string& strFilePath,unsigned int flg)
{
	Importer importer;
	auto ptrScene = importer.ReadFile(strFilePath, flg);
	if (!ptrScene || /*ptrScene->mFlags == AI_SCENE_FLAGS_INCOMPLETE ||*/ !ptrScene->mRootNode)
	{
		std::cout << "读取模型出现错误: " << importer.GetErrorString() << std::endl;
		exit(-1);
	}
	auto ptrRootNode = ptrScene->mRootNode;
	CreateMesh(ptrRootNode, ptrScene);
	
}

bool Model::CreateMesh(aiNode* ptrNode, const aiScene* ptrScene)
{
	bool bRet = false;
	if (!ptrNode || !ptrScene)
	{
		return bRet;
	}
	for (int i = 0; i < ptrNode->mNumMeshes; i++)
	{
		auto idx = ptrNode->mMeshes[i];
		auto ptrMesh = ptrScene->mMeshes[idx];
		Mesh m;
		for (auto j = 0; j < ptrMesh->mNumVertices; j++)
		{
			glm::vec3 vvv(0, 0, 0);
			vvv.x = ptrMesh->mVertices[j].x;
			vvv.y = ptrMesh->mVertices[j].y;
			vvv.z = ptrMesh->mVertices[j].z;
			m.m_vecPositon.push_back(vvv);

			if (ptrMesh->mNormals)
			{
				vvv.x = ptrMesh->mNormals[j].x;
				vvv.y = ptrMesh->mNormals[j].y;
				vvv.z = ptrMesh->mNormals[j].z;
				m.m_vecNormal.push_back(vvv);
			}

			if (ptrMesh->mTextureCoords)
			{
				glm::vec2 vv(0, 0);
				if (ptrMesh->mTextureCoords[0])
				{
					vv.x = ptrMesh->mTextureCoords[0][j].x;
					vv.y = ptrMesh->mTextureCoords[0][j].y;
					m.m_vecTexcoord.push_back(vv);
				}
			}

		}

		for (auto j = 0; j < ptrMesh->mNumFaces; j++)
		{
			auto face = ptrMesh->mFaces[j];
			for (auto k = 0; k < face.mNumIndices; k++)
			{
				//m.m_vecIndex.push_back(k);
				m.m_vecIndex.push_back(face.mIndices[k]);
			}
		}
		m.BindData();
		m_vecMesh.push_back(m);
	}
	for (int i = 0; i < ptrNode->mNumChildren; i++)
	{
		auto ptrChildNode = ptrNode->mChildren[i];
		CreateMesh(ptrChildNode, ptrScene);
	}
	bRet = true;
	return bRet;
}



void Model::Draw(GLuint program)
{
	for (auto& mesh : m_vecMesh)
	{
		mesh.Draw(program);
	}
}

float Model::GetBoundingBoxLength()
{
	float len = 0;
	glm::vec3 minPnt;
	glm::vec3 maxPnt;
	for (auto mesh : m_vecMesh)
	{
		auto& vecPositon = mesh.GetPosition();
		for (auto point : vecPositon)
		{
			if (point.x < minPnt.x)
				minPnt.x = point.x;
			if (point.y < minPnt.y)
				minPnt.y = point.y;
			if (point.z < minPnt.z)
				minPnt.z = point.z;
			if (point.x > maxPnt.x)
				maxPnt.x = point.x;
			if (point.y > maxPnt.y)
				maxPnt.y = point.y;
			if (point.z > maxPnt.z)
				maxPnt.z = point.z;
		}
	}
	m_center = (maxPnt + minPnt) * 0.5f;
	len = glm::distance(maxPnt, minPnt);
	return len;
}
