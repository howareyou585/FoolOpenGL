#ifndef READER_INCLUDE_
#define READER_INCLUDE_
#include<string>
#include <vector>
#include "Mesh.h"
#include <assimp/scene.h>
using namespace std;
class Model
{
public:
	vector<Mesh> m_vecMesh;
	map<string, GLuint> m_mapTexture;
	glm::vec3 m_center;
public :
	Model() {};
	virtual ~Model() {};
	void Load(const string& strFilePath, unsigned int flg);
	bool CreateMesh(aiNode* ptrNode,const aiScene* ptrScene);
	void Draw(GLuint program);
	float GetBoundingBoxLength();
	
};
#endif