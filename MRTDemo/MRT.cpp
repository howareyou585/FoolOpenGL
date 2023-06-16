// 引入GLEW库 定义静态链接
#define GLEW_STATIC
#include <GLEW/glew.h>
// 引入GLFW库
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

// 包含着色器加载库
#include <glm/glm.hpp>
#include "learnopengl/shader.h"
#include "learnopengl/model.h"
#include "learnopengl/filesystem.h"
#include "learnopengl/Maroc.h"
#include "learnopengl/vaoBuffer.h"
#include "learnopengl/vertexset.h"
// 键盘回调函数原型声明
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

// 定义程序常量
const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;

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
	// 创建缓存对象
	
	VAOBuffer vaoBuffer;
	
	vector<vertex_attribute> vecVertexAttrib;
	vecVertexAttrib.push_back(vertex_attribute::position);
	vecVertexAttrib.push_back(vertex_attribute::texcoord);
	map< vertex_attribute, int> mapAttrib2Num;
	mapAttrib2Num[vertex_attribute::position] = 3;
	mapAttrib2Num[vertex_attribute::texcoord] = 2;
	vaoBuffer.BuildVAO(squareVertices, sizeof(squareVertices), 
		squareIndexes, sizeof(squareIndexes), 
		vecVertexAttrib, mapAttrib2Num);
	GLuint VAOId, VBOId, EBOId;
	VAOId = vaoBuffer.GetVAO();
	VBOId = vaoBuffer.GetVBO();
	EBOId = vaoBuffer.GetEBO();

	//创建FBO
	GLuint mrtFrameBufferId;
	glGenFramebuffers(1, &mrtFrameBufferId);
	glBindFramebuffer(GL_FRAMEBUFFER, mrtFrameBufferId);
	const int nAttachment = 4;
	GLenum attatchments[nAttachment] = { 
		GL_COLOR_ATTACHMENT0, // 原图
		GL_COLOR_ATTACHMENT1, // 红色
		GL_COLOR_ATTACHMENT2, //绿色
		GL_COLOR_ATTACHMENT3 };// 蓝色

	GLuint textureIds[nAttachment] = {};
	glGenTextures(nAttachment, textureIds);
	for (auto i = 0; i < nAttachment; i++)
	{
		glBindTexture(GL_TEXTURE_2D, textureIds[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, attatchments[i], GL_TEXTURE_2D, textureIds[i], 0);
	}
	glDrawBuffers(nAttachment, attatchments);
	if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER))
	{
		//todo
	}
	//glGenTextures()
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Section2 准备着色器程序
	int imgHeight = 0;
	int imgWidth = 0;
	GLuint texId = TextureFromFile("cat.png","../resources/textures",imgWidth, imgHeight);
	Shader shader("triangle.vertex", "triangle.frag");
	Shader mrtShader("MRT.vertex", "MRT.frag");
	shader.use();
	GL_INPUT_ERROR
	shader.setInt("tex", 0);
	shader.unUse();

	mrtShader.use();
	for (int i = 0; i < nAttachment; i++)
	{
		string strname = "tex" + std::to_string(i);
		mrtShader.setInt(strname, i);
	}
	mrtShader.unUse();
	// 设置视口参数
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	// 开始游戏主循环
	// 清除颜色缓冲区 重置为指定颜色
	glClearColor(0.18f, 0.04f, 0.14f, 1.0f);
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents(); // 处理例如鼠标 键盘等事件
		glBindFramebuffer(GL_FRAMEBUFFER, mrtFrameBufferId);
		// 设置视口参数
		//glViewport(0, 0, imgWidth, imgHeight);
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawBuffers(nAttachment, attatchments);
		// 这里填写场景绘制代码
		glBindVertexArray(VAOId);
		shader.use();
		//必须先激活，再进行绑定。
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texId);
		
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, squareIndexes);
		shader.unUse();
		
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		//glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT);
		mrtShader.use();
		for (int i = 0; i < nAttachment; i++)
		{
			//必须先激活，再进行绑定。
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, textureIds[i]);
			
		}
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, squareIndexes);
		mrtShader.unUse();
		glBindVertexArray(0);
		glUseProgram(0);

		glfwSwapBuffers(window); // 交换缓存
	}
	// 释放资源
	glDeleteVertexArrays(1, &VAOId);
	glDeleteBuffers(1, &VBOId);
	glDeleteBuffers(1, &EBOId);
	glfwTerminate();
	return 0;
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE); // 关闭窗口
	}
}