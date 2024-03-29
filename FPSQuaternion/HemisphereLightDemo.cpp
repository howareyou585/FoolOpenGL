// 引入GLEW库 定义静态链接
#define GLEW_STATIC
#include <GLEW/glew.h>
// 引入GLFW库
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

// 包含着色器加载库
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include "learnopengl/shader.h"
#include "learnopengl/vertexset.h"
#include "learnopengl/vaoBuffer.h"
#include "learnopengl/model.h"
#include "learnopengl/camera.h"
#include "learnopengl/boundingbox.h"
#include "learnopengl/Maroc.h"
#include "learnopengl/light.h"
// 键盘回调函数原型声明
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

// 定义程序常量
const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;
bool bEnableAttenuation = true;

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

	
	// 创建缓存对象
	VAOBuffer vaoBuffer;
	vector<vertex_attribute> vecVertexAttrib;
	vecVertexAttrib.push_back(vertex_attribute::position);
	vecVertexAttrib.push_back(vertex_attribute::normal);
	vecVertexAttrib.push_back(vertex_attribute::texcoord);
	map< vertex_attribute, int> mapVertexAttrib2Num;
	mapVertexAttrib2Num[vertex_attribute::position] = 3;
	mapVertexAttrib2Num[vertex_attribute::normal] = 3;
	mapVertexAttrib2Num[vertex_attribute::texcoord] = 2;
	vaoBuffer.BuildVAO(cubeVertices, sizeof(cubeVertices),
		nullptr, 0, vecVertexAttrib, mapVertexAttrib2Num);
	GLuint VAOId = vaoBuffer.GetVAO();
	//准备材质
	GLuint diffuseTextureId = TextureFromFile("container_diffuse.png", "../resources/textures");
	GLuint spacularTexureId = TextureFromFile("container_specular.png", "../resources/textures");
	//设置相机
	BoundingBox box;
	int nVal = sizeof(cubeVertices) / sizeof(GLfloat);
	for(int i = 0; i< nVal; i+=8)
	{ 
		glm::vec3 pnt(cubeVertices[i], cubeVertices[i + 1], cubeVertices[i + 2]);
		box.Merge(pnt);
	}

	// positions all containers
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
	BoundingBox totalCubeBoundingBox;
	vector<glm::mat4>vecCubeModelMatrix;
	vecCubeModelMatrix.reserve(sizeof(cubePositions) / sizeof(glm::vec3));
	for (int i = 0; i < sizeof(cubePositions) / sizeof(glm::vec3); i++)
	{
		glm::mat4 tempModelMatrix(1.0f);
		tempModelMatrix = glm::translate(tempModelMatrix, cubePositions[i]);
		float angle = 20.0f * i;
		tempModelMatrix = glm::rotate(tempModelMatrix, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		auto tempModelBoundingBox = box.Transformed(tempModelMatrix);
		totalCubeBoundingBox.Merge(tempModelBoundingBox);
		vecCubeModelMatrix.emplace_back(tempModelMatrix);
	}
	
	Camera camera;
	camera.InitCamera(totalCubeBoundingBox, 0.8f);
	// Section2 准备着色器程序
	Shader shader("HemisphereLight.vertex", "HemisphereLight.frag");
	//设置平行光参数
	glm::vec3 ambimentColor (0.05f, 0.05f, 0.05f);
	glm::vec3 diffuseColor(0.4f, 0.4f, 0.4f);
	glm::vec3 specularColor(0.5f, 0.5f, 0.5f);
	glm::vec3 lightDir(-0.2f, -1.0f, -0.3f);
	Directionlight dirLight(ambimentColor,
		diffuseColor,
		specularColor, lightDir);
	glm::vec3 skyColor(0.8f, 0.8f, 0.8f);
	glm::vec3 groundCoor(0.3f, 0.3f, 0.3f);
	glm::vec3 hsDir = camera.Front;
	HemisphereLight hsLight(skyColor,groundCoor, hsDir);
	shader.use();
	dirLight.SetLightUniformParam(shader, "directionlight.");
	hsLight.SetLightUniformParam(shader, "hemisphereLight.");
	//设置材质参数
	shader.setInt("material.diffuse", 0);
	shader.setInt("material.spacular", 1);
	shader.setFloat("material.shiness", 256.0f);
	shader.unUse();

	float raduis = glm::length(camera.Position - totalCubeBoundingBox.GetCenter());
	// 
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	// 开始游戏主循环
	int nVertex = nVal / 8;
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents(); // 处理例如鼠标 键盘等事件

		// 清除颜色缓冲区 重置为指定颜色
		glClearColor(0.18f, 0.04f, 0.14f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		// 这里填写场景绘制代码
		glBindVertexArray(VAOId);
		shader.use();
		glm::mat4 model(1.f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = camera.GetProjectionMatrix((GLfloat)WINDOW_WIDTH / WINDOW_HEIGHT);
		
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		shader.setVec3("eyePos", camera.Position);
		
		float tv = glfwGetTime();

		glm::vec3 lightPos = camera.Position;
		lightPos.x = raduis * glm::sin(tv);
		lightPos.z = raduis * fabs(glm::cos(tv));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseTextureId);

		glActiveTexture(GL_TEXTURE0+1);
		glBindTexture(GL_TEXTURE_2D, spacularTexureId);

		for (int i = 0; i < vecCubeModelMatrix.size(); i++)
		{
			shader.setMat4("model", vecCubeModelMatrix[i]);
			glDrawArrays(GL_TRIANGLES, 0, nVertex);
		}
		
		glBindVertexArray(0);
		glUseProgram(0);

		glfwSwapBuffers(window); // 交换缓存
	}
	// 释放资源
	glDeleteVertexArrays(1, &VAOId);
	glfwTerminate();
	return 0;
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE); // 关闭窗口
	}
	else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		bEnableAttenuation = !bEnableAttenuation;
	}
}