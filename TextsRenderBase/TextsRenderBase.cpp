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
int IntilizeAscIIText(const string&  strText, int x, int y);
int IntilizeAscIIText2(const string& strText, int x, int y);
// ���̻ص�����ԭ������
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
GLuint vaoId{};
GLuint vboId{};

GLuint textureId{};
// ���������
const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;
struct Character {
	GLuint     TextureID;  // ���������ID
	glm::ivec2 Size;       // ���δ�С
	glm::ivec2 Bearing;    // �ӻ�׼�ߵ�������/������ƫ��ֵ
	GLuint     Advance;    // ԭ�����һ������ԭ��ľ���
};
std::map<GLchar, Character> mapCharacter;
struct CharBitmap
{
	glm::ivec2 Size;       // ���δ�С
	glm::ivec2 Bearing;    // �ӻ�׼�ߵ�������/������ƫ��ֵ
	GLuint     Advance;    // ԭ�����һ������ԭ��ľ���
	unsigned char* ptrBuffer;
};
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

	string stText("hello world");
	IntilizeAscIIText2(stText,25,25);
	// Section2 ׼����ɫ������
	Shader shader("TextsBase.vertex", "TextsBase.frag");
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(WINDOW_WIDTH), 0.0f, static_cast<float>(WINDOW_HEIGHT));
	shader.use();
	shader.setMat4("projection", projection);
	//glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glm::vec3 color(0.5f, 0.5f, 0.5f);
	shader.setVec3("textColor", color);
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
		glBindVertexArray(vaoId);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureId);
		shader.use();

		glDrawArrays(GL_TRIANGLES, 0, 6);
		shader.unUse();
		glfwSwapBuffers(window); // ��������
	}
	// �ͷ���Դ
	glDeleteVertexArrays(1, &vaoId);
	glDeleteBuffers(1, &vboId);
	glfwTerminate();
	return 0;
}
int IntilizeAscIIText(const string& strText,int x,int y)
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
	
	float minY = (1 << 16) - 1; // ��С��Y
	float maxY = 0.0f; //����Y
	float txtWidth = 0;
	float txtHeight = 0;
	
	//1.��ʼ���ַ�������
	vector<CharBitmap>vecCharacter;
	for (int i = 0; i < strText.length(); i++)
	{
		auto c = strText[i];
		//�����ַ���Ӧ������
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		FT_UInt glyphIndex = FT_Get_Char_Index(face,c);
		FT_Load_Glyph(face, glyphIndex, FT_LOAD_RENDER);
		//
		float top = face->glyph->bitmap_top;
		float btm = face->glyph->bitmap_top - face->glyph->bitmap.rows;
		if (maxY < top)
		{
			maxY = top;
		}
		if (minY > btm)
		{
			minY = btm;
		}
		//face->glyph->m
		txtWidth += (face->glyph->advance.x >> 6);
		CharBitmap charBitmap
		{
			glm::ivec2(face->glyph->bitmap.width,face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left,face->glyph->bitmap_top),
			face->glyph->advance.x,
			face->glyph->bitmap.buffer
		};
		vecCharacter.emplace_back(charBitmap);
	}
	txtHeight = maxY - minY;


	//�����ֽڶ�������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//��������
	
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RED,
		txtWidth,
		txtHeight,
		0,
		GL_RED,
		GL_UNSIGNED_BYTE,
		0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	glBindTexture(GL_TEXTURE_2D, textureId);
	int xOffset = 0;
	int yOffset = 0;
	for (auto it = vecCharacter.begin(); it != vecCharacter.end(); it++)
	{
		CharBitmap cb = *it;
		glTexSubImage2D(GL_TEXTURE_2D, 0, xOffset, yOffset, cb.Size.x, cb.Size.y, GL_RED, GL_UNSIGNED_BYTE, cb.ptrBuffer);
		xOffset += (face->glyph->advance.x >> 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	//����VBO

	GLfloat xpos = x + vecCharacter[0].Bearing.x;  //�ַ��������½� X����
	GLfloat ypos = y - (vecCharacter[0].Size.y - vecCharacter[0].Bearing.y);//�ַ��������½� Y����
	GLfloat w = txtWidth;
	GLfloat h = txtHeight;
	
	//����VBO
	GLfloat vertices[6][4] =
	{
		{xpos ,    ypos + h,	0.0f,	0.0f}, // ���Ͻ�
		{xpos,     ypos,		0.0,	1.0f}, //���½�
		{xpos + w,   ypos,		1.0f,	1.0f},//���½�

		{xpos,     ypos + h,   0.0,		0.0}, // ���Ͻ�
		{xpos + w, ypos,       1.0,		1.0}, // ���½�
		{xpos + w, ypos + h,   1.0,		0.0}  // ���Ͻ�
	};

	glBindVertexArray(vaoId);
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

int IntilizeAscIIText2(const string& strText, int x, int y)
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

	float minY = (1 << 16) - 1; // ��С��Y
	float maxY = 0.0f; //����Y
	float minX = minY; //��С��X
	float maxX = maxY; //����X
	
	int ppy = 0;

	FT_Vector penPos;
	//FT_UInt glyphIndex;
	penPos.x = 0;
	penPos.y = 0;

	bool bFirst = true;
	for (int i = 0; i < strText.length(); i++)
	{
		auto c = strText[i];
		//�����ַ���Ӧ������
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		FT_UInt glyphIndex = FT_Get_Char_Index(face, c);
		FT_Load_Glyph(face, glyphIndex, FT_LOAD_RENDER);
		auto py = face->glyph->bitmap.rows - face->glyph->bitmap_top;
		for (int y = face->glyph->bitmap.rows - 1; y >= 0; y--)
		{
			for (int x = 0; x < face->glyph->bitmap.width; x++)
			{
				int tx = penPos.x + face->glyph->bitmap_left + x;
				int ty = penPos.y + py - (face->glyph->bitmap.rows - y);
				if (bFirst)
				{
					bFirst = false;
					minX = tx;
					maxX = tx;
					minY = ty;
					maxY = ty;
					ppy = py;
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
					if (ppy < py)
					{
						ppy = py;
					}
				}
			}
		}
		penPos.x += (face->glyph->advance.x >> 6);
		
	}
	glm::vec2 size(maxX - minX, maxY - minY);
	size.x += 20;
	size.y += 20;
	int txtWidth = size.x;
	int txtHeight = size.y;

	int length = txtWidth * txtHeight;

	unsigned char* ptrTextData = new unsigned char[length];
	memset(ptrTextData, 0, length);

	FT_UInt glyphIndex;
	FT_Vector vectorPen;
	vectorPen.x = 10;
	vectorPen.y = txtHeight - 10 - ppy;

	for (auto iter = strText.begin(); iter != strText.end(); iter++)
	{
		if (FT_Load_Char(face, *iter, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		auto c = *iter;
		glyphIndex = FT_Get_Char_Index(face, c);
		FT_Load_Glyph(face, glyphIndex, FT_LOAD_RENDER);
		FT_Bitmap* bitmap = &face->glyph->bitmap;
		auto Py = face->glyph->bitmap.rows - face->glyph->bitmap_top;
		for (int y = bitmap->rows - 1; y >= 0; --y) {

			for (int x = 0; x < bitmap->width; x++)
			{
				int tx = vectorPen.x + face->glyph->bitmap_left + x;
				int ty = vectorPen.y + Py - (face->glyph->bitmap.rows - y);
				int idx = ty * txtWidth + tx;
				ptrTextData[idx] = bitmap->buffer[y * bitmap->width + x];

			}
		}
		vectorPen.x += (face->glyph->advance.x >> 6);
	}
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	unsigned int texture2;
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RED,
		txtWidth,
		txtHeight,
		0,
		GL_RED,
		GL_UNSIGNED_BYTE,
		ptrTextData
	);
	// set texture options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	Character character = {
				texture2,
				glm::vec2(txtWidth ,txtHeight),
				glm::vec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				static_cast<unsigned int>(face->glyph->advance.x)
	};
	mapCharacter.insert(std::pair<unsigned int, Character>(10000, character));

	if (ptrTextData)
	{
		delete[]ptrTextData;
		ptrTextData = nullptr;
	}
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	// -----------------------------------
	glGenVertexArrays(1, &vaoId);
	glGenBuffers(1, &vboId);
	glBindVertexArray(vaoId);
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

	//��ʼ��VBO		
	int scale = 1.0;
	Character ch = mapCharacter[10000];
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

	return 0;
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE); // �رմ���
	}
}