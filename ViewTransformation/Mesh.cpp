#include "Mesh.h"

void Mesh::BindData()
{
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
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
	//normal
	GLuint offset_normal = size_position;
	GLuint size_normal = m_vecNormal.size() * sizeof(glm::vec3);
	glBufferSubData(GL_ARRAY_BUFFER, offset_normal, size_normal, m_vecNormal.data());

	//textcoord
	GLuint offset_textcoord = size_position + size_normal;
	GLuint size_textcoord = m_vecTexcoord.size() * sizeof(glm::vec2);
	glBufferSubData(GL_ARRAY_BUFFER, offset_textcoord, size_textcoord, m_vecTexcoord.data());

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,0, (void*)0);
	
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
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_vecIndex.size(), GL_UNSIGNED_INT, m_vecIndex.data());
	glBindVertexArray(0);
}
