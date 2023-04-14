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
	const auto ptrScene = importer.ReadFile(strFilePath, flg);
	if (!ptrScene || /*ptrScene->mFlags == AI_SCENE_FLAGS_INCOMPLETE ||*/ !ptrScene->mRootNode)
	{
		std::cout << "读取模型出现错误: " << importer.GetErrorString() << std::endl;
		exit(-1);
	}
	for (int i = 0; i < ptrScene->mNumMeshes; i++)
	{
		auto ptrMesh = ptrScene->mMeshes[i];
		Mesh m;
		for (auto j = 0; j < ptrMesh->mNumVertices; j++)
		{
			glm::vec3 vvv(0,0,0);
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

			if(ptrMesh->mTextureCoords)
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
				m.m_vecIndex.push_back(k);
			}
		}
		m.BindData();
		m_vecMesh.push_back(m);
	}
}

void Model::Draw(GLuint program)
{
	for (auto& mesh : m_vecMesh)
	{
		mesh.Draw(program);
	}
}
