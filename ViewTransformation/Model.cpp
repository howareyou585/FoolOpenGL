#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/material.h>
#include <glm/glm.hpp>
#include <GLEW/glew.h>
#include <iostream>
using namespace Assimp;
using namespace glm; 
void Model::Load(const string& strFilePath,unsigned int flg)
{
	Importer importer;
	auto ptrScene = importer.ReadFile(strFilePath, flg);
	if (!ptrScene || ptrScene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !ptrScene->mRootNode)
	{
		std::cout << "Error::ASSIMP:: " << importer.GetErrorString() << std::endl;
		exit(-1);
	}
	auto ptrRootNode = ptrScene->mRootNode;
	ProcessNode(ptrRootNode, ptrScene);
	
}

bool Model::ProcessNode(aiNode* ptrNode, const aiScene* ptrScene)
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
		Mesh m = ProcessMesh(ptrMesh, ptrScene);
		
		m.BindData();
		m_vecMesh.push_back(m);
	}
	for (int i = 0; i < ptrNode->mNumChildren; i++)
	{
		auto ptrChildNode = ptrNode->mChildren[i];
		ProcessNode(ptrChildNode, ptrScene);
	}
	bRet = true;
	return bRet;
}
Mesh  Model::ProcessMesh(aiMesh* ptrMesh, const aiScene* ptrScene)
{
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
		if (ptrMesh->mMaterialIndex >= 0)
		{
			aiMaterial* ptrMaterial = ptrScene->mMaterials[ptrMesh->mMaterialIndex];
			//todo
			LoadMaterialTextures(ptrMaterial, aiTextureType::aiTextureType_DIFFUSE, "texture_diffuse");
			LoadMaterialTextures(ptrMaterial, aiTextureType::aiTextureType_SPECULAR, "texture_specular");
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
	return m;
}

void Model::LoadMaterialTextures(aiMaterial* ptrMaterial, aiTextureType type, const string& typeName)
{
	for (auto i = 0; i < ptrMaterial->GetTextureCount(type); i++)
	{
		aiString str;
		ptrMaterial->GetTexture(type, i, &str);
		string strPath(str.C_Str());
	}
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
