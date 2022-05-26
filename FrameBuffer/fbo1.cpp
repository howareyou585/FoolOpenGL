// 引入GLEW库 定义静态链接
#define GLEW_STATIC
#include <GLEW/glew.h>
// 引入GLFW库
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// 包含着色器加载库
#include "shader.h"
#include "texture.h"

// 键盘回调函数原型声明
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

// 定义程序常量
const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;
GLuint PrepareFBO2(GLuint& textureId, GLuint&depthStencilTextId)
{
	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	//GLuint textureId;
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureId, 0);
	/*depthStencilTextId = TextureHelper::makeAttachmentTexture(0, GL_DEPTH24_STENCIL8,
		WINDOW_WIDTH, WINDOW_HEIGHT, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8);
	glBindTexture(GL_TEXTURE_2D, 0);*/
	//GLuint rboId;
	//glGenRenderbuffers(1, &rboId);
	//glBindRenderbuffer(GL_RENDERBUFFER, rboId);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
	//	WINDOW_WIDTH, WINDOW_HEIGHT); // 预分配内存
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboId);

	glGenRenderbuffers(1, &depthStencilTextId);
	glBindRenderbuffer(GL_RENDERBUFFER, depthStencilTextId);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WINDOW_WIDTH, WINDOW_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH24_STENCIL8, GL_RENDERBUFFER, depthStencilTextId);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		return 0;
	}
	
	return fbo;
}
bool PrepareFBO(GLuint& colorTextId, GLuint& depthStencilTextId, GLuint& fboId)
{
	glGenFramebuffers(1, &fboId);
	glBindFramebuffer(GL_FRAMEBUFFER, fboId);
	colorTextId = TextureHelper::makeAttachmentTexture(0, GL_RGB, WINDOW_WIDTH,
		WINDOW_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE); // 创建FBO中的纹理
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTextId, 0);
	// 附加 depth stencil texture attachment
	depthStencilTextId = TextureHelper::makeAttachmentTexture(0, GL_DEPTH24_STENCIL8,
		WINDOW_WIDTH, WINDOW_HEIGHT, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
		GL_TEXTURE_2D, depthStencilTextId, 0); 
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER)!=GL_FRAMEBUFFER_COMPLETE)
	{
		return false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER,0);
	return true;
}
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

	//Section1 顶点属性数据
	// 指定立方体顶点属性数据 顶点位置 颜色 纹理
	GLfloat cubeVertices[] = {
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,	// A
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f,	// B
		0.5f, 0.5f, 0.5f,1.0f, 1.0f,	// C
		0.5f, 0.5f, 0.5f,1.0f, 1.0f,	// C
		-0.5f, 0.5f, 0.5f,0.0f, 1.0f,	// D
		-0.5f, -0.5f, 0.5f,0.0f, 0.0f,	// A


		-0.5f, -0.5f, -0.5f,0.0f, 0.0f,	// E
		-0.5f, 0.5f, -0.5f,0.0, 1.0f,   // H
		0.5f, 0.5f, -0.5f,1.0f, 1.0f,	// G
		0.5f, 0.5f, -0.5f,1.0f, 1.0f,	// G
		0.5f, -0.5f, -0.5f,1.0f, 0.0f,	// F
		-0.5f, -0.5f, -0.5f,0.0f, 0.0f,	// E

		-0.5f, 0.5f, 0.5f,0.0f, 1.0f,	// D
		-0.5f, 0.5f, -0.5f,1.0, 1.0f,   // H
		-0.5f, -0.5f, -0.5f,1.0f, 0.0f,	// E
		-0.5f, -0.5f, -0.5f,1.0f, 0.0f,	// E
		-0.5f, -0.5f, 0.5f,0.0f, 0.0f,	// A
		-0.5f, 0.5f, 0.5f,0.0f, 1.0f,	// D

		0.5f, -0.5f, -0.5f,1.0f, 0.0f,	// F
		0.5f, 0.5f, -0.5f,1.0f, 1.0f,	// G
		0.5f, 0.5f, 0.5f,0.0f, 1.0f,	// C
		0.5f, 0.5f, 0.5f,0.0f, 1.0f,	// C
		0.5f, -0.5f, 0.5f, 0.0f, 0.0f,	// B
		0.5f, -0.5f, -0.5f,1.0f, 0.0f,	// F

		0.5f, 0.5f, -0.5f,1.0f, 1.0f,	// G
		-0.5f, 0.5f, -0.5f,0.0, 1.0f,   // H
		-0.5f, 0.5f, 0.5f,0.0f, 0.0f,	// D
		-0.5f, 0.5f, 0.5f,0.0f, 0.0f,	// D
		0.5f, 0.5f, 0.5f,1.0f, 0.0f,	// C
		0.5f, 0.5f, -0.5f,1.0f, 1.0f,	// G

		-0.5f, -0.5f, 0.5f,0.0f, 0.0f,	// A
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,// E
		0.5f, -0.5f, -0.5f,1.0f, 1.0f,	// F
		0.5f, -0.5f, -0.5f,1.0f, 1.0f,	// F
		0.5f, -0.5f, 0.5f,1.0f, 0.0f,	// B
		-0.5f, -0.5f, 0.5f,0.0f, 0.0f,	// A
	};
	// 地板顶点属性数据 顶点位置 纹理坐标(设置的值大于1.0用于重复)
	GLfloat planeVertices[] = {
		5.0f, -0.5f, 5.0f, 2.0f, 0.0f,   // A
		5.0f, -0.5f, -5.0f, 2.0f, 2.0f,  // D
		-5.0f, -0.5f, -5.0f, 0.0f, 2.0f, // C

		-5.0f, -0.5f, -5.0f, 0.0f, 2.0f, // C
		-5.0f, -0.5f, 5.0f, 0.0f, 0.0f,  // B
		5.0f, -0.5f, 5.0f, 2.0f, 0.0f,   // A
	};
	// 用于绘制FBO纹理的矩形顶点属性数据
	GLfloat quadVertices[] = {
		// 位置 纹理坐标
		-1.0f, 1.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 0.0f,

		-1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f
	};
	// 创建缓存对象
	GLuint cubeVAOId, cubeVBOId;
	// Step1: 创建并绑定VAO对象
	glGenVertexArrays(1, &cubeVAOId);
	glBindVertexArray(cubeVAOId);
	// Step2: 创建并绑定VBO对象
	glGenBuffers(1, &cubeVBOId);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBOId);
	// Step3: 分配空间 传送数据
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	// Step4: 指定解析方式  并启用顶点属性
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (GLvoid*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// 创建缓存对象
	GLuint planeVAOId, planeVBOId;
	// Step1: 创建并绑定VAO对象
	glGenVertexArrays(1, &planeVAOId);
	glBindVertexArray(planeVAOId);
	// Step2: 创建并绑定VBO对象
	glGenBuffers(1, &planeVBOId);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBOId);
	// Step3: 分配空间 传送数据
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
	// Step4: 指定解析方式  并启用顶点属性
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (GLvoid*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// 创建缓存对象
	GLuint quadVAOId, quadVBOId;
	// Step1: 创建并绑定VAO对象
	glGenVertexArrays(1, &quadVAOId);
	glBindVertexArray(quadVAOId);
	// Step2: 创建并绑定VBO对象
	glGenBuffers(1, &quadVBOId);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBOId);
	// Step3: 分配空间 传送数据
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
	// Step4: 指定解析方式  并启用顶点属性
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (GLvoid*)(2 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	GLuint textureId = TextureHelper::load2DTexture("../resources/textures/container.jpg");
	GLuint planeTexturId = TextureHelper::load2DTexture("../resources/textures/metal.png");
	// Section2 准备着色器程序
	Shader shader("scene.vertex", "scene.frag");
	Shader quadShader("quad.vertex", "quad.frag");
	glm::mat4 view = glm::lookAt(glm::vec3(0.3f,0.f,15.0f), glm::vec3(0.f,0.f,0.f),glm::vec3(0.f,1.0f,0.0f));
	glm::mat4 model;
	glm::mat4 projection = glm::perspective(50.f, WINDOW_WIDTH*1.0f / WINDOW_HEIGHT, 1.0f, 100.0f);
	// 开始游戏主循环
	
	GLuint colorTextId; 
	GLuint depthStencilTextId; 
	GLuint fboId;
	//PrepareFBO(colorTextId, depthStencilTextId, fboId);
	fboId = PrepareFBO2(colorTextId, depthStencilTextId);
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents(); // 处理例如鼠标 键盘等事件
		//使用自定义FBO 绘制
		glBindFramebuffer(GL_FRAMEBUFFER, fboId);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		// 清除颜色缓冲区 重置为指定颜色
		glClearColor(0.18f, 0.04f, 0.14f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		// 这里填写场景绘制代码
		
		shader.use();
		glBindVertexArray(cubeVAOId);
		//绘制第一个立方体
		glm::mat4 model;
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
		glUniformMatrix4fv(glGetUniformLocation(shader.programId, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(shader.programId, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(shader.programId, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureId);
		glUniform1i(glGetUniformLocation(shader.programId, "text"), 0);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		//绘制第二个立方体
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(shader.programId, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		//绘制平面
		glBindVertexArray(planeVAOId);
		model = glm::mat4();
		glBindTexture(GL_TEXTURE_2D, planeTexturId);
		glUniformMatrix4fv(glGetUniformLocation(shader.programId, "model"), 1, GL_FALSE, glm::value_ptr(model));
		
		glDrawArrays(GL_TRIANGLES, 0, 6);

		//使用默认的FBO绘图
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		quadShader.use();

		glBindVertexArray(quadVAOId);
		glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, colorTextId);
		glBindTexture(GL_TEXTURE_2D, depthStencilTextId);
		glUniform1i(glGetUniformLocation(quadShader.programId, "text"), 0);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		glUseProgram(0);

		glfwSwapBuffers(window); // 交换缓存
	}
	// 释放资源
	glDeleteVertexArrays(1, &cubeVAOId);
	glDeleteBuffers(1, &cubeVBOId);
	glDeleteVertexArrays(1, &planeVAOId);
	glDeleteBuffers(1, &planeVBOId);
	glDeleteVertexArrays(1, &quadVAOId);
	glDeleteBuffers(1, &quadVBOId);
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