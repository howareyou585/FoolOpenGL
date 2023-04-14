#ifndef READER_INCLUDE_
#define READER_INCLUDE_
#include<string>
#include <vector>
#include "Mesh.h"
using namespace std;
class Model
{
public:
	vector<Mesh> m_vecMesh;
	map<string, GLuint> m_mapTexture;
public :
	Model() {};
	virtual ~Model() {};
	void Load(const string& strFilePath, unsigned int flg);
	void Draw(GLuint program);
};
#endif