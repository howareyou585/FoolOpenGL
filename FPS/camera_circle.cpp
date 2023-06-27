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
#include "learnopengl/vaoBuffer.h"
#include "learnopengl/vertexset.h"
#include "learnopengl/boundingbox.h"
#include "learnopengl/model.h"
#include "learnopengl/camera.h"
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
	VAOBuffer vaoBuffer;
	vector<vertex_attribute> vecAttrib;
	map<vertex_attribute, int> mapAttrib2Size;
	vecAttrib.emplace_back(vertex_attribute::position);
	vecAttrib.emplace_back(vertex_attribute::texcoord);
	mapAttrib2Size[vertex_attribute::position] = 3;
	mapAttrib2Size[vertex_attribute::texcoord] = 2;
	vaoBuffer.BuildVAO(cubeVertices3, sizeof(cubeVertices3), nullptr, 
		0, vecAttrib, mapAttrib2Size);
	
	// 创建缓存对象
	GLuint VAOId = vaoBuffer.GetVAO();
	GLuint VBOId = vaoBuffer.GetVBO();
	//加载材质
	GLuint texId  = TextureFromFile("container.jpg", "../resources/textures");
	GLuint texId2 = TextureFromFile("awesomeface.png", "../resources/textures");
	// world space positions of our cubes
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};
	BoundingBox box;
	int nVal = sizeof(cubeVertices3) / sizeof(GLfloat);
	for (int i = 0; i < nVal; i += 5)
	{
		glm::vec3 pnt(cubeVertices3[i], cubeVertices3[i + 1], cubeVertices3[i + 2]);
		box.Merge(pnt);
	}
	BoundingBox totalBoundingBox;
	int nModelMatrix = sizeof(cubePositions) / sizeof(glm::vec3);
	vector<glm::mat4> vecModelMatrix;
	vecModelMatrix.reserve(nModelMatrix);
	for (int i = 0; i < nModelMatrix; i++)
	{
		glm::mat4 model(1.0f);
		model = glm::translate(model, cubePositions[i]);
		float angle = 20.0f * i;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		BoundingBox tmpBoundingBox = box.Transformed(model);
		totalBoundingBox.Merge(tmpBoundingBox);
		vecModelMatrix.emplace_back(model);
	}
	//float raduis = totalBoundingBox.GetLength()*0.8f;
	Camera camera;
	camera.InitCamera(totalBoundingBox,0.8f);
	float radius = totalBoundingBox.GetLength() * 0.8f;
	glm::mat4 projectionMatrix = camera.GetProjectionMatrix((float)WINDOW_WIDTH / (float)WINDOW_HEIGHT);
	// Section2 准备着色器程序
	Shader shader("cube.vertex", "cube.frag");
	shader.use();
	shader.setInt("s_texture", 0);
	shader.setInt("s_texture2", 1);
	shader.setMat4("projection", projectionMatrix);
	shader.unUse();
	int nVertex = sizeof(cubeVertices3) / (sizeof(GLuint) * 5);
	// 开始游戏主循环
	glEnable(GL_DEPTH_TEST);
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents(); // 处理例如鼠标 键盘等事件

		// 清除颜色缓冲区 重置为指定颜色
		glClearColor(0.18f, 0.04f, 0.14f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		// 这里填写场景绘制代码
		glBindVertexArray(VAOId);
		shader.use();
		glm::vec3 pnt = camera.Position;
		float timeValue = glfwGetTime();
		pnt.x = radius * glm::sin(timeValue);
		pnt.z = radius * glm::cos(timeValue);
		camera.SetPosition(pnt);
		//将相机设置为看向（0，0，0）
		//glm::mat4 viewMatrix = glm::lookAt(camera.Position, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		
		glm::mat4 viewMatrix = camera.GetViewMatrix(glm::vec3(0.0f,0.0f,0.0f));
		shader.setMat4("view", viewMatrix);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texId);
		glActiveTexture(GL_TEXTURE0+1);
		glBindTexture(GL_TEXTURE_2D, texId2);
		for (auto j = 0; j < nModelMatrix; j++)
		{
			shader.setMat4("model", vecModelMatrix[j]);
			glDrawArrays(GL_TRIANGLES, 0, nVertex);
		}
		shader.unUse();
		glBindVertexArray(0);
		glUseProgram(0);

		glfwSwapBuffers(window); // 交换缓存
	}
	// 释放资源
	glDeleteVertexArrays(1, &VAOId);
	glDeleteBuffers(1, &VBOId);
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