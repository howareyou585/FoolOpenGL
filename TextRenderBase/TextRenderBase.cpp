// ����GLEW�� ���徲̬����
#define GLEW_STATIC
#include <GLEW/glew.h>
// ����GLFW��
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <map>
// ������ɫ�����ؿ�
#include "glm/glm.hpp"
#include "learnopengl/shader.h"
#include "learnopengl/vaobuffer.h"
#include "ft2build.h"
#include FT_FREETYPE_H
void RenderText( Shader& shader, const string&text, float x, float y, float scale, glm::vec3 color);
// ���̻ص�����ԭ������
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
GLuint vaoId{};
GLuint vboId{};
// ���������
const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;
struct Character {
	GLuint     TextureID;  // ���������ID
	glm::ivec2 Size;       // ���δ�С
	glm::ivec2 Bearing;    // �ӻ�׼�ߵ�������/������ƫ��ֵ
	GLuint     Advance;    // ԭ�����һ������ԭ��ľ���
};
std::map<GLchar, Character> mapCharacter;
int main(int argc, char** argv)
{
	
	if (!glfwInit())	// ��ʼ��glfw��
	{
		std::cout << "Error::GLFW could not initialize GLFW!" << std::endl;
		return -1;
	}

	// ����OpenGL 3.3 core profile
	std::cout << "Start OpenGL core profile version 3.3" << std::endl;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// ��������
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
		"Demo of triangle", NULL, NULL);
	if (!window)
	{
		std::cout << "Error::GLFW could not create winddow!" << std::endl;
		glfwTerminate();
		return -1;
	}
	// �����Ĵ��ڵ�contextָ��Ϊ��ǰcontext
	glfwMakeContextCurrent(window);

	// ע�ᴰ�ڼ����¼��ص�����
	glfwSetKeyCallback(window, key_callback);

	// ��ʼ��GLEW ��ȡOpenGL����
	glewExperimental = GL_TRUE; // ��glew��ȡ������չ����
	GLenum status = glewInit();
	if (status != GLEW_OK)
	{
		std::cout << "Error::GLEW glew version:" << glewGetString(GLEW_VERSION) 
			<< " error string:" << glewGetErrorString(status) << std::endl;
		glfwTerminate();
		return -1;
	}

	// �����ӿڲ���
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		return -1;
	}
	FT_Face face;
	if (FT_New_Face(ft, "../resources/fonts/arial.ttf",0, &face))
	{
		return -1;
	}
	FT_Set_Pixel_Sizes(face, 0, 48);
	//�����ֽڶ�������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//׼��128���ַ�����������
	for (GLubyte c = 0; c < 128; c++)
	{
		//�����ַ���Ӧ������
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		//��������
		GLuint textureId;
		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		Character character = {
			textureId,
			glm::vec2(face->glyph->bitmap.width,face->glyph->bitmap.rows),
			glm::vec2(face->glyph->bitmap_left,face->glyph->bitmap_top)
		};
		mapCharacter[c] = character;
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
	// Section1 ׼����������
	// ָ�������������� ����λ��
	GLfloat vertices[24] = {
		
	};
	VAOBuffer vaoBuffer;
	vector<vertex_attribute> vecAttrib;
	map<vertex_attribute, int> mapAttrib2Size;
	vecAttrib.emplace_back(vertex_attribute::position);
	vecAttrib.emplace_back(vertex_attribute::texcoord);

	mapAttrib2Size[vertex_attribute::position] = 2;
	mapAttrib2Size[vertex_attribute::texcoord] = 2;

	vaoBuffer.BuildVAO(vertices, sizeof(vertices), nullptr, 0, vecAttrib, mapAttrib2Size);
	// �����������
	vaoId = vaoBuffer.GetVAO();
	vboId = vaoBuffer.GetVBO();
	// Step1: ��������VAO����
	

	// Section2 ׼����ɫ������
	Shader shader("TextBase.vertex", "TextBase.frag");

	// ��ʼ��Ϸ��ѭ��
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents(); // ����������� ���̵��¼�

		// �����ɫ������ ����Ϊָ����ɫ
		glClearColor(0.18f, 0.04f, 0.14f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//// ������д�������ƴ���
		//glBindVertexArray(vaoId);
		//shader.use();
		//glDrawArrays(GL_TRIANGLES, 0, 3);

		//glBindVertexArray(0);
		//glUseProgram(0);
		RenderText(shader, "Hello World!", 25, 25, 1.0f, glm::vec3(0, 0, 0));

		glfwSwapBuffers(window); // ��������
	}
	// �ͷ���Դ
	glDeleteVertexArrays(1, &vaoId);
	glDeleteBuffers(1, &vboId);
	glfwTerminate();
	return 0;
}
void RenderText( Shader& shader, const string&text, float x, float y, float scale, glm::vec3 color)
{
	glBindVertexArray(vaoId);
	shader.use();

	for (int i = 0; i < text.length(); i++)
	{
		Character c = mapCharacter[text[i]];
		GLfloat xpos = x + c.Bearing.x * scale; //�ַ������½� X����
		GLfloat ypos = y - (c.Size.y - c.Bearing.y) * scale;//�ַ������½� Y����
		GLfloat w = c.Size.x * scale;
		GLfloat h = c.Size.y * scale;
		//����VBO
		GLfloat vertices[6][4] =
		{
			{xpos , ypos + h, 0.0f,0.0f}, // ���Ͻ�
			{xpos,ypos,0.0,1.0f}, //���½�
			{xpos+w, ypos,1.0f,1.0f},//���½�
			{xpos,     ypos + h,   0.0, 0.0}, // ���Ͻ�
			{xpos + w, ypos,       1.0, 1.0}, // ���½�
			{xpos + w, ypos + h,   1.0, 0.0}  // ���Ͻ�
		};
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, c.TextureID);
		glBindBuffer(GL_ARRAY_BUFFER, vboId);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		x +=( c.Advance >> 6) * scale; // ��ͳһ��λ��Advance�ĵ�λΪ���ص�1/64, С��λ����ֵ=����λС��ֵ
	}

	shader.unUse();
	glBindVertexArray(0);
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE); // �رմ���
	}
}