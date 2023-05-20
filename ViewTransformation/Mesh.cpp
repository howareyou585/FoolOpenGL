#include "Mesh.h"

void Mesh::BindData()
{
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	//使用一个VBO保存位置 法向 纹理坐标
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	auto length = m_vecPositon.size() * sizeof(glm::vec3) +
				  m_vecNormal.size() * sizeof(glm::vec3) +
				  m_vecTexcoord.size()*sizeof(glm::vec2);
	glBufferData(GL_ARRAY_BUFFER, m_vecPositon.size() * sizeof(glm::vec3), nullptr, GL_STATIC_DRAW);
	//position
	GLuint offset_positon = 0;
	GLuint size_position = m_vecPositon.size() * sizeof(glm::vec3);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size_position, m_vecPositon.data());
	////normal
	GLuint offset_normal = size_position;
	GLuint size_normal = m_vecNormal.size() * sizeof(glm::vec3);
	glBufferSubData(GL_ARRAY_BUFFER, offset_normal, size_normal, m_vecNormal.data());

	////textcoord
	GLuint offset_textcoord = size_position + size_normal;
	GLuint size_textcoord = m_vecTexcoord.size() * sizeof(glm::vec2);
	glBufferSubData(GL_ARRAY_BUFFER, offset_textcoord, size_textcoord, m_vecTexcoord.data());

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)offset_normal);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)offset_textcoord);
	

	glGenBuffers(1, &m_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_vecIndex.size() * sizeof(GLuint), m_vecIndex.data(), GL_STATIC_DRAW);
	glBindVertexArray(0);
}
void Mesh::Draw(GLuint program)
{
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	unsigned int normalNr = 1;
	unsigned int heightNr = 1;
	for (unsigned int i = 0; i < m_vecTexture.size(); i++)
	{
		string strNumber;
		string strName = m_vecTexture[i].type;
		if(strName == "texture_diffuse")
		{
			strNumber = std::to_string(diffuseNr);
			strName = strName + strName;
			diffuseNr++;
			
		}
		else if (strName == "texture_specular")
		{
			strNumber = std::to_string(specularNr);
			strName = strName + strName;
			specularNr++;
		}
		else if (strName == "textue_normal")
		{
			strNumber = std::to_string(normalNr);
			strName = strName + strName;
			normalNr++;
		}
		else if (strName == "textue_height")
		{
			strNumber = std::to_string(heightNr);
			strName = strName + strName;
			heightNr++;
		}
		else
		{
			continue;
		}
		glActiveTexture(GL_TEXTURE0 + i);
		glUniform1i(glGetUniformLocation(program, strName.c_str()), i);
		glBindTexture(GL_TEXTURE_2D, m_vecTexture[i].id);
	}

	glBindVertexArray(m_vao);
	//glDrawElements(GL_TRIANGLES, m_vecIndex.size(), GL_UNSIGNED_INT, m_vecIndex.data());//这样写没有效果
	glDrawElements(GL_TRIANGLES, m_vecIndex.size(), GL_UNSIGNED_INT, 0);//为什么上面的不对，这样写才对
	//glDrawArrays(GL_TRIANGLES, 0, m_vecPositon.size());
	glBindVertexArray(0);
}
void Mesh::BindVAO()
{
	glBindVertexArray(m_vao);
}
void Mesh::UnBindVAO()
{
	glBindVertexArray(0);
}

vector<glm::vec3>& Mesh::GetPosition()
{
	return m_vecPositon;
}