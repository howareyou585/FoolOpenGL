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
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "learnopengl/shader.h"
#include "learnopengl/vaobuffer.h"
#include "ft2build.h"
#include FT_FREETYPE_H
float RenderText( Shader& shader, const string&text, float x, float y, float scale, glm::vec3 color);
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
		glTexImage2D(
			GL_TEXTURE_2D, 
			0, 
			GL_RED,
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
			glm::ivec2(face->glyph->bitmap.width,face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left,face->glyph->bitmap_top),
			face->glyph->advance.x
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
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(WINDOW_WIDTH), 0.0f, static_cast<float>(WINDOW_HEIGHT));
	shader.use();
	shader.setMat4("projection", projection);
	//glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	shader.unUse();
	// ��ʼ��Ϸ��ѭ��
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents(); // ����������� ���̵��¼�

		// �����ɫ������ ����Ϊָ����ɫ
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//// ������д�������ƴ���
		//glBindVertexArray(vaoId);
		//shader.use();
		//glDrawArrays(GL_TRIANGLES, 0, 3);

		//glBindVertexArray(0);
		//glUseProgram(0);
		string str = "Hello World!";
		//glm::vec3(0.5f, 0.8f, 0.2f)
		glm::vec3 color(0.5f,0.5f,0.5f);
		float y = 25.f;
		for (int i = 0; i < 5; i++)
		{
			float height = RenderText(shader, str, 25, y, 1.0f, color);
			y += height*1.2f;
		}
		glfwSwapBuffers(window); // ��������
	}
	// �ͷ���Դ
	glDeleteVertexArrays(1, &vaoId);
	glDeleteBuffers(1, &vboId);
	glfwTerminate();
	return 0;
}
// render line of text
// -------------------
//void RenderText(Shader &shader, const std::string& text, float x, float y, float scale, glm::vec3 color)
//{
//	// activate corresponding render state	
//	shader.use();
//	glUniform3f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y, color.z);
//	glActiveTexture(GL_TEXTURE0);
//	glBindVertexArray(vaoId);
//
//	// iterate through all characters
//	std::string::const_iterator c;
//	for (c = text.begin(); c != text.end(); c++)
//	{
//		Character ch = mapCharacter[*c];
//
//		float xpos = x + ch.Bearing.x * scale;
//		float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
//
//		float w = ch.Size.x * scale;
//		float h = ch.Size.y * scale;
//		// update VBO for each character
//		float vertices[6][4] = {
//			{ xpos,     ypos + h,   0.0f, 0.0f },
//			{ xpos,     ypos,       0.0f, 1.0f },
//			{ xpos + w, ypos,       1.0f, 1.0f },
//
//			{ xpos,     ypos + h,   0.0f, 0.0f },
//			{ xpos + w, ypos,       1.0f, 1.0f },
//			{ xpos + w, ypos + h,   1.0f, 0.0f }
//		};
//		// render glyph texture over quad
//		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
//		// update content of VBO memory
//		glBindBuffer(GL_ARRAY_BUFFER, vboId);
//		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData
//
//		glBindBuffer(GL_ARRAY_BUFFER, 0);
//		// render quad
//		glDrawArrays(GL_TRIANGLES, 0, 6);
//		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
//		x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
//	}
//	glBindVertexArray(0);
//	glBindTexture(GL_TEXTURE_2D, 0);
//}

float RenderText( Shader& shader, const string&text, float x, float y, float scale, glm::vec3 color)
{
	glBindVertexArray(vaoId);
	glActiveTexture(GL_TEXTURE0);
	shader.use();
	shader.setVec3("textColor", color);
	float maxHeight{};
	for (int i = 0; i < text.length(); i++)
	{
		Character c = mapCharacter[text[i]];
		GLfloat xpos = x + c.Bearing.x * scale; //�ַ������½� X����
		GLfloat ypos = y - (c.Size.y - c.Bearing.y) * scale;//�ַ������½� Y����
		GLfloat w = c.Size.x * scale;
		GLfloat h = c.Size.y * scale;
		maxHeight = maxHeight < h ? h : maxHeight;
		//����VBO
		GLfloat vertices[6][4] =
		{
			{xpos ,    ypos + h,	0.0f,	0.0f}, // ���Ͻ�
			{xpos,     ypos,		0.0,	1.0f}, //���½�
			{xpos+w,   ypos,		1.0f,	1.0f},//���½�

			{xpos,     ypos + h,   0.0,		0.0}, // ���Ͻ�
			{xpos + w, ypos,       1.0,		1.0}, // ���½�
			{xpos + w, ypos + h,   1.0,		0.0}  // ���Ͻ�
		};
		
		glBindTexture(GL_TEXTURE_2D, c.TextureID);
		glBindBuffer(GL_ARRAY_BUFFER, vboId);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); //�˴���sizeof(vertices) д�� size(vertices) �����˺þ�û���ҵ�����
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		x +=( c.Advance >> 6) * scale; // ��ͳһ��λ��Advance�ĵ�λΪ���ص�1/64, С��λ����ֵ=����λС��ֵ
	}

	shader.unUse();
	glBindVertexArray(0);
	return maxHeight;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE); // �رմ���
	}
}