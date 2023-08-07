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

	//创建 plane缓存对象
	float planeVertices[] = {
		// positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
		 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

		 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
		 5.0f, -0.5f, -5.0f,  2.0f, 2.0f
	};
	VAOBuffer planeVaoBuffer;
	planeVaoBuffer.BuildVAO(planeVertices, sizeof(planeVertices), nullptr,
		0, vecVertexAttrib, mapAttrib2Num);


	GLuint planeVAOId = planeVaoBuffer.GetVAO();
	GLuint planeVBOId = planeVaoBuffer.GetVBO();

	BoundingBox bbox(cubeVertices3, sizeof(cubeVertices3)/sizeof(float), 5);
	
	//创建纹理对象
	GLuint boxTexId = TextureFromFile("marble.jpg", "../resources/textures");
	GLuint planeTexId = TextureFromFile("metal.png", "../resources/textures");
	// Section2 准备着色器程序
	
	Shader cubeShader("cube.vertex", "cube.frag");
	Shader planeShader("cube.vertex", "cube.frag");
	
	//创建相机
	Camera camera;
	camera.InitCamera(bbox, 2.5f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	/*glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xff); 
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);*/

	// 开始游戏主循环
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 projection = camera.GetProjectionMatrix((float)WINDOW_WIDTH/(float)WINDOW_HEIGHT);
	cubeShader.use();
	cubeShader.setMat4("view", view);
	cubeShader.setMat4("projection", projection);
	cubeShader.setInt("tex", 0);
	cubeShader.unUse();
	glDisable(GL_CULL_FACE);
	planeShader.use();
	planeShader.setMat4("view", view);
	planeShader.setMat4("projection", projection);
	planeShader.setInt("tex", 0);
	planeShader.unUse();
	
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents(); // 处理例如鼠标 键盘等事件

		// 清除颜色缓冲区 重置为指定颜色
		glClearColor(0.18f, 0.04f, 0.14f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
		glm::mat4 model;
		//1.绘制地板
		glBindVertexArray(planeVAOId);
		planeShader.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, planeTexId);
		planeShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		planeShader.unUse();
		//2.绘制箱子
		
		cubeShader.use();
		glBindVertexArray(cubeVAOId);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, boxTexId);
		
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