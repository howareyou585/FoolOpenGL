#include "../GLEW/glew.h"
#include <vector>
using namespace std;
typedef struct
{
	unsigned long long offset{};
	unsigned long long length{};
} 
EBOOffeset;

typedef struct
{
	GLuint count;
	GLuint primCount;
	GLuint firstIndex;
	GLuint baseVertex;
	GLuint baseInstance;
} DrawElementsIndirectCommand;

class  IndirectCommandBuffer 
{
	
public:
	IndirectCommandBuffer(vector<EBOOffeset>& vecEBOOffset)
	{
		auto nCmd = vecEBOOffset.size();
		m_nCommand = nCmd;
		auto error = glGetError();
		if (error != 0)
		{
			std::cout << "error no: " << error << std::endl;
		}
		glGenBuffers(1, &m_objectId);
		error = glGetError();
		if (error != 0)
		{
			std::cout << "error no: " << error << std::endl;
		}
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_objectId);
		error = glGetError();
		if (error != 0)
		{
			std::cout << "error no: " << error << std::endl;
		}
		glBufferStorage(GL_DRAW_INDIRECT_BUFFER, nCmd * sizeof(DrawElementsIndirectCommand), nullptr, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
		error = glGetError();
		if (error != 0)
		{
			std::cout << "error no: " << error << std::endl;
		}
		DrawElementsIndirectCommand* ptrIndirectCommand = (DrawElementsIndirectCommand*)glMapBufferRange(GL_DRAW_INDIRECT_BUFFER,
			0,
			nCmd * sizeof(DrawElementsIndirectCommand),
			GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
		error = glGetError();
		if (error != 0)
		{
			std::cout << "error no: " << error << std::endl;
		}
		if (ptrIndirectCommand)
		{
			for (size_t i = 0; i < vecEBOOffset.size(); i++)
			{
				ptrIndirectCommand[i].firstIndex = vecEBOOffset[i].offset;
				ptrIndirectCommand[i].count = vecEBOOffset[i].length;
				ptrIndirectCommand[i].primCount = 1;
				ptrIndirectCommand[i].baseVertex = 0;
				ptrIndirectCommand[i].baseInstance = i;
			}
		}
		glUnmapBuffer(GL_DRAW_INDIRECT_BUFFER);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

	}
	virtual ~IndirectCommandBuffer()
	{
		
	}
	void Bind()
	{
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_objectId);
	}
	void UnBind()
	{
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
	}
	
	int GetCmdCount()
	{
		return m_nCommand;
	}
private:
	//CommandType m_cmdType;
	DrawElementsIndirectCommand m_ptrIndirectCommand{};
	GLuint m_objectId{};
	int m_nCommand{};
};