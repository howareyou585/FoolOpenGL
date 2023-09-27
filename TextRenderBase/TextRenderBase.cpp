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
#include  <codecvt>
#include "ft2build.h"
#include FT_FREETYPE_H
int IntilizeAscIIText();
int IntilizeAscIITexts(string& text);

float RenderText(Shader& shader, const string&text,   float x, float y, float scale, glm::vec3 color);
void RenderTexts(Shader& shader, const string& text,  float x, float y, float scale, glm::vec3 color);
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
	IntilizeAscIIText();
	/*string str("hahhaaa");
	IntilizeUnicodeText(str);*/
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

		string str = "Hello World!";
		glm::vec3 color(0.5f,0.5f,0.5f);
		float y = 25.f;
		for (int i = 0; i < 5; i++)
		{
			float height = RenderText(shader, str, 25, y, 1.0f, color);
			y += height*1.2f;
		}
		/*wstring strtext = L"�����й���";
		RenderText(shader, strtext, 25, 25, 1.0f, color);*/
		glfwSwapBuffers(window); // ��������
	}
	// �ͷ���Դ
	glDeleteVertexArrays(1, &vaoId);
	glDeleteBuffers(1, &vboId);
	glfwTerminate();
	return 0;
}

void RenderTexts(Shader& shader, const string& text, float x, float y, float scale, glm::vec3 color)
{
	glBindVertexArray(vaoId);
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	//��ʼ��VBO		
	//int scale = 1.0;
	Character ch =mapCharacter[10000];
	float xpos = 0;
	float ypos = 0;
	float w = ch.Size.x * scale;
	float h = ch.Size.y * scale;
	// update VBO for each character
	float vertices[6][4] = {
		{ xpos,     ypos + h,   0.0f, 0.0f },
		{ xpos,     ypos,       0.0f, 1.0f },
		{ xpos + w, ypos,       1.0f, 1.0f },

		{ xpos,     ypos + h,   0.0f, 0.0f },
		{ xpos + w, ypos,       1.0f, 1.0f },
		{ xpos + w, ypos + h,   1.0f, 0.0f }
	};

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	glBindVertexArray(vaoId);
	glBindTexture(GL_TEXTURE_2D, ch.TextureID);
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); //�˴���sizeof(vertices) д�� size(vertices) �����˺þ�û���ҵ�����
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}
int IntilizeAscIIText()
{
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		return -1;
	}
	FT_Face face;
	if (FT_New_Face(ft, "../resources/fonts/arial.ttf", 0, &face))
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
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
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
	//FT_Matrix matrix;
	//FT_Vector delta;
	//delta.x = 100;
	//delta.y = 100;
	//FT_Set_Transform(face, &matrix, &delta);
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

int IntilizeAscIITexts(string& text)
{
	int ret = -1;
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
	}

	FT_Face face;
	if (FT_New_Face(ft, "../resources/fonts/arial.ttf", 0, &face)) 
	{
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
	}
	//FT_Set_Charmap(face,FT_Get_System_Charmap());

	FT_Set_Pixel_Sizes(face, 0, 40);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	FT_Vector ftVector;
	FT_UInt index = -1;
	ftVector.x = 0;
	ftVector.y = 0;
	int minX = (1<<16)-1;
	int maxX = 0;
	int minY = (1<<16)-1;
	int maxY = 0;
	int PPY = 0;
	//SIZE
	int i = 0;
	bool Isfirst = true;
	for (auto it = text.begin(); it != text.end(); it++)
	{
		auto c = *it;

		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}

		index = FT_Get_Char_Index(face, c);

		FT_Load_Glyph(face, index, FT_LOAD_RENDER);

		FT_Bitmap* bitmap = &face->glyph->bitmap;

		auto Py = face->glyph->bitmap.rows - face->glyph->bitmap_top;
		for (int y = bitmap->rows - 1; y >= 0; --y) 
		{

			for (int x = 0; x < bitmap->width; x++)
			{

				int tx = ftVector.x + face->glyph->bitmap_left + x;
				int ty = ftVector.y + Py - (face->glyph->bitmap.rows - y);

				if (Isfirst)
				{
					Isfirst = false;
					minX = tx;
					maxX = tx;
					minY = ty;
					maxY = ty;
					PPY = Py;
				}
				else
				{
					maxX = tx;
					if (maxY < ty)
					{
						maxY = ty;
					}
					if (minY > ty)
					{
						minY = ty;
					}
					if (PPY < Py)
					{
						PPY = Py;
					}
				}

			}
		}

		ftVector.x += (face->glyph->advance.x >> 6);
	}
	//�����ַ����ĸ߶��볤��
	glm::vec2 size(maxX - minX, maxY - minY);
	//M3D::Math::Vector2 SIZE(MaxX - MinX, MaxY - MinY);
	size.x += 20;
	size.y += 20;

	//TEX
	int texWidth = size.x;
	int texHeight = size.y;
	unsigned char* texData = new unsigned char[texWidth * texHeight];
	memset(texData, 0, texWidth * texHeight);
	//
	FT_UInt glyphIndex;
	FT_Vector pen;
	pen.x = 10;
	pen.y = texHeight - 10 - PPY;
	i = 0;
	for (auto it = text.begin(); it != text.end(); it++)
	{
		if (FT_Load_Char(face, *it, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		auto c = *it;
		glyphIndex = FT_Get_Char_Index(face, c);
		FT_Load_Glyph(face, glyphIndex, FT_LOAD_RENDER);
		FT_Bitmap* bitmap = &face->glyph->bitmap;
		auto Py = face->glyph->bitmap.rows - face->glyph->bitmap_top;
		for (int y = bitmap->rows - 1; y >= 0; --y) {

			for (int x = 0; x < bitmap->width; x++)
			{
				int tx = pen.x + face->glyph->bitmap_left + x;
				int ty = pen.y + Py - (face->glyph->bitmap.rows - y);
				int idx = ty * texWidth + tx;
				texData[idx] = bitmap->buffer[y * bitmap->width + x];

			}
		}
		pen.x += (face->glyph->advance.x >> 6);
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	unsigned int texture2;
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RED,
		texWidth,
		texHeight,
		0,
		GL_RED,
		GL_UNSIGNED_BYTE,
		texData
	);
	// set texture options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	Character character = {
				texture2,
				glm::vec2(texWidth ,texHeight),
				glm::vec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				static_cast<unsigned int>(face->glyph->advance.x)
	};
	mapCharacter.insert(std::pair<unsigned int, Character>(10000, character));

	FT_Done_Face(face);
	FT_Done_FreeType(ft);
	delete[]texData;
	texData = nullptr;
	ret = 0;
	return ret;
}
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