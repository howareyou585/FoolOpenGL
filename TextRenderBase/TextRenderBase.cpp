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
#include "learnopengl/shader.h"
#include "learnopengl/vaobuffer.h"
#include "ft2build.h"
#include FT_FREETYPE_H
void RenderText(const Shader& shader, const string&text, float x, float y, float scale, glm::vec3 color);
// 键盘回调函数原型声明
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

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
	GLuint vaoId = vaoBuffer.GetVAO();
	GLuint vboId = vaoBuffer.GetVBO();
	// Step1: 创建并绑定VAO对象
	

	// Section2 准备着色器程序
	Shader shader("TextBase.vertex", "TextBase.frag");

	// 开始游戏主循环
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents(); // 处理例如鼠标 键盘等事件

		// 清除颜色缓冲区 重置为指定颜色
		glClearColor(0.18f, 0.04f, 0.14f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// 这里填写场景绘制代码
		glBindVertexArray(vaoId);
		shader.use();
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glBindVertexArray(0);
		glUseProgram(0);

		glfwSwapBuffers(window); // 交换缓存
	}
	// 释放资源
	glDeleteVertexArrays(1, &vaoId);
	glDeleteBuffers(1, &vboId);
	glfwTerminate();
	return 0;
}
void RenderText(const Shader& shader, const string&text, float x, float y, float scale, glm::vec3 color)
{

}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE); // 关闭窗口
	}
}