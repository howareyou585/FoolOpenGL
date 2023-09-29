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
int IntilizeAscIIText(const string&  strText, int x, int y);
// 键盘回调函数原型声明
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
GLuint vaoId{};
GLuint vboId{};

GLuint textureId{};
// 定义程序常量
const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;
//struct Character {
//	GLuint     TextureID;  // 字形纹理的ID
//	glm::ivec2 Size;       // 字形大小
//	glm::ivec2 Bearing;    // 从基准线到字形左部/顶部的偏移值
//	GLuint     Advance;    // 原点距下一个字形原点的距离
//};
//std::map<GLchar, Character> mapCharacter;
struct CharBitmap
{
	glm::ivec2 Size;       // 字形大小
	glm::ivec2 Bearing;    // 从基准线到字形左部/顶部的偏移值
	GLuint     Advance;    // 原点距下一个字形原点的距离
	unsigned char* ptrBuffer;
};
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

	string stText("hello world");
	IntilizeAscIIText(stText,25,25);
	// Section2 准备着色器程序
	Shader shader("TextsBase.vertex", "TextsBase.frag");
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(WINDOW_WIDTH), 0.0f, static_cast<float>(WINDOW_HEIGHT));
	shader.use();
	shader.setMat4("projection", projection);
	//glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glm::vec3 color(0.5f, 0.5f, 0.5f);
	shader.setVec3("textColor", color);
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
		glBindVertexArray(vaoId);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureId);
		shader.use();

		glDrawArrays(GL_TRIANGLES, 0, 6);
		shader.unUse();
		glfwSwapBuffers(window); // 交换缓存
	}
	// 释放资源
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
	
	float minY = (1 << 16) - 1; // 最小的Y
	float maxY = 0.0f; //最大的Y
	float txtWidth = 0;
	float txtHeight = 0;
	
	//1.初始化字符串纹理
	vector<CharBitmap>vecCharacter;
	for (int i = 0; i < strText.length(); i++)
	{
		auto c = strText[i];
		//加载字符对应的字形
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
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


	//禁用字节对齐限制
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//生成纹理
	
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

	//更新VBO

	GLfloat xpos = x + vecCharacter[0].Bearing.x;  //字符串的左下角 X分量
	GLfloat ypos = y - (vecCharacter[0].Size.y - vecCharacter[0].Bearing.y);//字符串的左下角 Y分量
	GLfloat w = txtWidth;
	GLfloat h = txtHeight;
	
	//更新VBO
	GLfloat vertices[6][4] =
	{
		{xpos ,    ypos + h,	0.0f,	0.0f}, // 左上角
		{xpos,     ypos,		0.0,	1.0f}, //左下角
		{xpos + w,   ypos,		1.0f,	1.0f},//右下角

		{xpos,     ypos + h,   0.0,		0.0}, // 左上角
		{xpos + w, ypos,       1.0,		1.0}, // 右下角
		{xpos + w, ypos + h,   1.0,		0.0}  // 右上角
	};

	glBindVertexArray(vaoId);
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE); // 关闭窗口
	}
}