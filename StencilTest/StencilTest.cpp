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
#include "learnopengl/vaobuffer.h"
#include "learnopengl/model.h"
#include "learnopengl/vertexset.h"
#include "learnopengl/camera.h"
#include "learnopengl/boundingbox.h"

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

	// 设置视口参数
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	// Section1 准备顶点数据
	// 指定顶点属性数据 顶点位置

	//	// 模板矩形
	GLfloat stencilVertices[] = {
		-0.5f, -0.5f, 0.5f, // A
		-0.5f, 0.5f, 0.5f,	// B
		-1.5f, 0.5f, 0.5f,  // C

		-1.5f, 0.5f, 0.5f,  // C
		-1.5f, -0.5f, 0.5f, // D
		-0.5f, -0.5f, 0.5f  // A
	};
	// 创建cube缓存对象
	VAOBuffer cubeVaoBuffer;
	vector<vertex_attribute> vecVertexAttrib;
	vecVertexAttrib.push_back(vertex_attribute::position);
	vecVertexAttrib.push_back(vertex_attribute::texcoord);
	map< vertex_attribute, int> mapAttrib2Num;
	mapAttrib2Num[vertex_attribute::position] = 3;
	mapAttrib2Num[vertex_attribute::texcoord] = 2;
	cubeVaoBuffer.BuildVAO(cubeVertices3, sizeof(cubeVertices3),
		nullptr, 0,
		vecVertexAttrib, mapAttrib2Num);
	GLuint cubeVAOId = cubeVaoBuffer.GetVAO();
	GLuint cubeVBOId = cubeVaoBuffer.GetVBO();

	BoundingBox bbox(cubeVertices3, sizeof(cubeVertices3)/sizeof(float), 5);
	// 创建stencil缓存对象
	VAOBuffer stencilVaoBuffer;
	vecVertexAttrib.clear();
	mapAttrib2Num.clear();
	vecVertexAttrib.push_back(vertex_attribute::position);
	mapAttrib2Num[vertex_attribute::position] = 3;
	stencilVaoBuffer.BuildVAO(stencilVertices, sizeof(stencilVertices),
		nullptr, 0,
		vecVertexAttrib, mapAttrib2Num);
	GLuint stencilVAOId = stencilVaoBuffer.GetVAO();
	GLuint stencilVBOId = stencilVaoBuffer.GetVBO();
	//创建纹理对象
	GLuint textureId = TextureFromFile("bricks2.jpg", "../resources/textures");
	// Section2 准备着色器程序
	Shader stencilShader("singleColor.vertex", "singleColor.frag");
	Shader cubeShader("cube.vertex", "cube.frag");

	
	//创建相机
	Camera camera;
	camera.InitCamera(bbox, 1.5f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_STENCIL_TEST);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	// 开始游戏主循环
	
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 projection = camera.GetProjectionMatrix((float)WINDOW_WIDTH/(float)WINDOW_HEIGHT);
	cubeShader.use();
	cubeShader.setMat4("view", view);
	cubeShader.setMat4("projection", projection);
	cubeShader.setInt("tex", 0);
	cubeShader.unUse();

	stencilShader.use();
	stencilShader.setMat4("view", view);
	stencilShader.setMat4("projection", projection);
	stencilShader.unUse();
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents(); // 处理例如鼠标 键盘等事件

		// 清除颜色缓冲区 重置为指定颜色
		glClearColor(0.18f, 0.04f, 0.14f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
		glm::mat4 model;
		//1.绘制模板
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE,GL_FALSE);// 禁止向颜色缓冲区中写颜色=》导致颜色缓冲区为黑色
		glDepthMask(GL_FALSE);//禁止向深度缓冲区中写深度值。

		glStencilMask(0xff);
		glStencilFunc(GL_ALWAYS, 1, 0xff);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

		
		// 这里填写场景绘制代码
		glBindVertexArray(stencilVAOId);
		stencilShader.use();
		
		stencilShader.setMat4 ("model",model);
		
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);// 禁止向颜色缓冲区中写颜色=》导致颜色缓冲区为黑色
		glDepthMask(GL_TRUE);//禁止向深度缓冲区中写深度值。

		glStencilMask(0x00);
		glStencilFunc(GL_EQUAL, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		//
		cubeShader.use();
		glBindVertexArray(cubeVAOId);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureId);
		
		model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
		cubeShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
		cubeShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		//glStencilMask(0xFF);  // 再次允许写入模板缓冲区 以便下次迭代时清除
		glBindVertexArray(0);
		glUseProgram(0);

		glfwSwapBuffers(window); // 交换缓存
	}
	// 释放资源
	glDeleteVertexArrays(1, &stencilVAOId);
	glDeleteBuffers(1, &stencilVBOId);
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