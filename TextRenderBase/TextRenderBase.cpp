// 引入GLEW库 定义静态链接
#define GLEW_STATIC
#include <GLEW/glew.h>
// 引入GLFW库
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <map>
// 包含着色器加载库
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
// 键盘回调函数原型声明
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
GLuint vaoId{};
GLuint vboId{};
// 定义程序常量
const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;
struct Character {
	GLuint     TextureID;  // 字形纹理的ID
	glm::ivec2 Size;       // 字形大小
	glm::ivec2 Bearing;    // 从基准线到字形左部/顶部的偏移值
	GLuint     Advance;    // 原点距下一个字形原点的距离
};
std::map<GLchar, Character> mapCharacter;
int main(int argc, char** argv)
{
	
	if (!glfwInit())	// 初始化glfw库
	{
		std::cout << "Error::GLFW could not initialize GLFW!" << std::endl;
		return -1;
	}

	// 开启OpenGL 3.3 core profile
	std::cout << "Start OpenGL core profile version 3.3" << std::endl;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// 创建窗口
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
		"Demo of triangle", NULL, NULL);
	if (!window)
	{
		std::cout << "Error::GLFW could not create winddow!" << std::endl;
		glfwTerminate();
		return -1;
	}
	// 创建的窗口的context指定为当前context
	glfwMakeContextCurrent(window);

	// 注册窗口键盘事件回调函数
	glfwSetKeyCallback(window, key_callback);

	// 初始化GLEW 获取OpenGL函数
	glewExperimental = GL_TRUE; // 让glew获取所有拓展函数
	GLenum status = glewInit();
	if (status != GLEW_OK)
	{
		std::cout << "Error::GLEW glew version:" << glewGetString(GLEW_VERSION) 
			<< " error string:" << glewGetErrorString(status) << std::endl;
		glfwTerminate();
		return -1;
	}

	// 设置视口参数
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	IntilizeAscIIText();
	/*string str("hahhaaa");
	IntilizeUnicodeText(str);*/
	// Section1 准备顶点数据
	// 指定顶点属性数据 顶点位置
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
	// 创建缓存对象
	vaoId = vaoBuffer.GetVAO();
	vboId = vaoBuffer.GetVBO();
	// Step1: 创建并绑定VAO对象
	

	// Section2 准备着色器程序
	Shader shader("TextBase.vertex", "TextBase.frag");
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(WINDOW_WIDTH), 0.0f, static_cast<float>(WINDOW_HEIGHT));
	shader.use();
	shader.setMat4("projection", projection);
	//glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	shader.unUse();
	// 开始游戏主循环
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents(); // 处理例如鼠标 键盘等事件

		// 清除颜色缓冲区 重置为指定颜色
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
		/*wstring strtext = L"我是中国人";
		RenderText(shader, strtext, 25, 25, 1.0f, color);*/
		glfwSwapBuffers(window); // 交换缓存
	}
	// 释放资源
	glDeleteVertexArrays(1, &vaoId);
	glDeleteBuffers(1, &vboId);
	glfwTerminate();
	return 0;
}

void RenderTexts(Shader& shader, const string& text, float x, float y, float scale, glm::vec3 color)
{
	glBindVertexArray(vaoId);
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	//初始化VBO		
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
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); //此处把sizeof(vertices) 写成 size(vertices) 调试了好久没有找到问题
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
	
	//禁用字节对齐限制
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//准备128个字符的纹理数据
	for (GLubyte c = 0; c < 128; c++)
	{
		//加载字符对应的字形
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		//生成纹理
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
	//计算字符串的高度与长度
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
		GLfloat xpos = x + c.Bearing.x * scale; //字符的左下角 X分量
		GLfloat ypos = y - (c.Size.y - c.Bearing.y) * scale;//字符的左下角 Y分量
		GLfloat w = c.Size.x * scale;
		GLfloat h = c.Size.y * scale;
		maxHeight = maxHeight < h ? h : maxHeight;
		//更新VBO
		GLfloat vertices[6][4] =
		{
			{xpos ,    ypos + h,	0.0f,	0.0f}, // 左上角
			{xpos,     ypos,		0.0,	1.0f}, //左下角
			{xpos+w,   ypos,		1.0f,	1.0f},//右下角

			{xpos,     ypos + h,   0.0,		0.0}, // 左上角
			{xpos + w, ypos,       1.0,		1.0}, // 右下角
			{xpos + w, ypos + h,   1.0,		0.0}  // 右上角
		};
		
		glBindTexture(GL_TEXTURE_2D, c.TextureID);
		glBindBuffer(GL_ARRAY_BUFFER, vboId);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); //此处把sizeof(vertices) 写成 size(vertices) 调试了好久没有找到问题
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		x +=( c.Advance >> 6) * scale; // （统一单位）Advance的单位为像素的1/64, 小单位大数值=》大单位小数值
	}

	shader.unUse();
	glBindVertexArray(0);
	return maxHeight;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE); // 关闭窗口
	}
}