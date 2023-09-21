// 引入GLEW库 定义静态链接
#define GLEW_STATIC
#include <GLEW/glew.h>
// 引入GLFW库
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

// 包含着色器加载库
#include "learnopengl/shader.h"
#include "learnopengl/vaobuffer.h"
#include "learnopengl/model.h"

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
	GLfloat vertices[] = {
		//position				//color                  //texture_coord
		1.0f,3.f,0.f,           1.f,0.f,0.f,			 1.f,2.f,
		1.0f,-3.f, 0.f,         0.f,1.f,0.f,		     1.f,-1.f,
		-1.0f, -3.f, 0.f,		0.f,0.f,1.f,			 0.f,-1.f,
		-1.0f, 3.f, 0.f,		0.f,0.f,0.f,			 0.f,2.f
	};
	GLuint indices[] = { 0,1,3,1,2,3 };
	// 创建缓存对象
	vector<vertex_attribute> vecAttrib;
	map<vertex_attribute, int> mapAttrib2Size;
	vecAttrib.emplace_back(vertex_attribute::position);
	vecAttrib.emplace_back(vertex_attribute::color);
	vecAttrib.emplace_back(vertex_attribute::texcoord);
	mapAttrib2Size[vertex_attribute::position] = 3;
	mapAttrib2Size[vertex_attribute::color] = 3;
	mapAttrib2Size[vertex_attribute::texcoord] = 2;
	VAOBuffer vaoBuffer;
	vaoBuffer.BuildVAO(vertices, sizeof(vertices), indices, sizeof(indices), vecAttrib, mapAttrib2Size);
	GLuint vaoId = vaoBuffer.GetVAO();
	GLuint vboId = vaoBuffer.GetVBO();
	GLuint texSnow1 = TextureFromFile("snow1.png", "../resources/textures");
	GLuint texSnow2 = TextureFromFile("snow2.png", "../resources/textures");
	// Section2 准备着色器程序
	Shader shader("Snowing.vertex", "Snowing.frag");
	shader.use();
	shader.setInt("s_tex1", 0);
	shader.setInt("s_tex2", 1);
	shader.unUse();
	int cnt = 0;
	int flag = 1;
	int N = 2000;
	// 开始游戏主循环
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents(); // 处理例如鼠标 键盘等事件

		// 清除颜色缓冲区 重置为指定颜色
		glClearColor(0.18f, 0.04f, 0.14f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// 这里填写场景绘制代码
		vaoBuffer.Bind();
		vaoBuffer.BindEBO();
		//glBindVertexArray(vaoId);
		shader.use();
		
		glActiveTexture( GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texSnow1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D,texSnow2);

		cnt += 1;
		if (cnt > 2 * N)
		{
			cnt = 0;
		}
		if (cnt == 0 || cnt == N)
		{
			flag = -flag;
		}
		float time = 0.f;
		if (flag == 1)
		{
			time = cnt * 1.0f / N;
			
		}
		else
		{
			time = 2.f - cnt * 1.0f / N;
		}
		shader.setFloat("time", time);
		glm::mat4 transform(1.f);
		transform = glm::translate(transform, glm::vec3(0.f, 1.0 - cnt * 1.0 / N, 0.0f));
		shader.setMat4("transform", transform);
		//glDrawElements(GL_TRIANGLES,6 ,GL_UNSIGNALED,indices);
		glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(sizeof(indices)), GL_UNSIGNED_INT, 0);
		vaoBuffer.UnBindEBO();
		vaoBuffer.UnBind();
		glUseProgram(0);

		glfwSwapBuffers(window); // 交换缓存
	}
	// 释放资源
	glDeleteVertexArrays(1, &vaoId);
	glDeleteBuffers(1, &vboId);
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