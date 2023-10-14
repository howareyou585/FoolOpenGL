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
// 键盘回调函数原型声明
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

// 定义程序常量
const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;
GLfloat heightScale = 0.1f;
bool CreateVertexSet(GLfloat** ptrBuffer, int& count);

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

	GLfloat* ptrVertexBuffer = nullptr;
	int count = 0;
	if (!CreateVertexSet(&ptrVertexBuffer, count))
	{
		return -1;
	}
	// 创建缓存对象
	VAOBuffer vaoBuffer;
	vector<vertex_attribute> vecVertexAttrib;
	//pos1.x, pos1.y, pos1.z, 
	//nm.x, nm.y, nm.z, 
	//uv1.x, uv1.y, 
	//tangent1.x, tangent1.y, tangent1.z, 
	//bitangent1.x, bitangent1.y, bitangent1.z
	vecVertexAttrib.push_back(vertex_attribute::position);
	vecVertexAttrib.push_back(vertex_attribute::normal);
	vecVertexAttrib.push_back(vertex_attribute::texcoord);
	vecVertexAttrib.push_back(vertex_attribute::tangent);
	vecVertexAttrib.push_back(vertex_attribute::bitangent);

	map< vertex_attribute, int> mapVertexAttrib2Num;
	mapVertexAttrib2Num[vertex_attribute::position] = 3;
	mapVertexAttrib2Num[vertex_attribute::normal] = 3;
	mapVertexAttrib2Num[vertex_attribute::texcoord] = 2;
	mapVertexAttrib2Num[vertex_attribute::tangent] = 3;
	mapVertexAttrib2Num[vertex_attribute::bitangent] = 3;

	vaoBuffer.BuildVAO(ptrVertexBuffer, count*sizeof(GLfloat),
		nullptr, 0, vecVertexAttrib, mapVertexAttrib2Num);
	GLuint VAOId = vaoBuffer.GetVAO();
	//准备材质
	GLuint diffuseMap = TextureFromFile("bricks2.jpg", "../resources/textures");
	GLuint normalMap  = TextureFromFile("bricks2_normal.jpg", "../resources/textures");
	GLuint heightMap = TextureFromFile("bricks2_disp.jpg", "../resources/textures");
	//设置相机
	BoundingBox box;
	int nVal = count;
	for(int i = 0; i< nVal; i+=14)
	{ 
		glm::vec3 pnt(ptrVertexBuffer[i], ptrVertexBuffer[i + 1], ptrVertexBuffer[i + 2]);
		box.Merge(pnt);
	}
	glm::vec3 center = box.GetCenter();
	glm::vec3 position = center +  (box.GetLength()*0.59f)*glm::vec3(0, 0, 1.0f);
	Camera camera(position);
	// Section2 准备着色器程序
	Shader shader("ParallaxMapping.vertex", "ParallaxMapping.frag");
	
	// 
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	// 开始游戏主循环
	GLfloat angle = 0.0f;
	
	int nVeretx = nVal / 14;
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
		angle += 0.25;
		if (angle >= 360)
		{
			angle -= 360;
		}
		float rad = glm::radians(angle);
		glm::quat q = glm::quat(glm::vec3(rad, rad, rad));
		model = glm::mat4_cast(q)*model;
		BoundingBox currentBox = box.Transformed(model);
		glm::vec3 currentCenter = box.GetCenter();
		

		//camera.Position = currentCenter+ (box.GetLength())*glm::vec3(0, 0, 1.0f);
		camera.Front = glm::normalize(currentCenter - camera.Position);
	
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(45.0f, (GLfloat)WINDOW_WIDTH / WINDOW_HEIGHT, 0.01f, 1000.0f);
		shader.setMat4("model", model);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		shader.setVec3("eyePos", camera.Position);
	
		
		glm::vec3 ambient(0.3f, 0.3f, 0.3f);
		glm::vec3 diffuse(0.8f, 0.8f, 0.8f);
		glm::vec3 spacular(1.0f, 1.0f, 1.0f);

		
		shader.setVec3("light.ambient", ambient);
		shader.setVec3("light.diffuse", diffuse);
		shader.setVec3("light.spacular", spacular);
		shader.setVec3("material.ambient", ambient);
		shader.setInt("material.diffuse", 0);
		shader.setVec3("material.spacular", spacular);
		shader.setFloat("material.shiness", 256.0f);

		shader.setInt("normalMap", 1);
		shader.setInt("depthMap", 2);
		shader.setFloat("heightScale", heightScale);
		glm::vec3 lightPos = glm::vec3(1.2f, 1.0f, 2.0f);
		shader.setVec3("lightPos", lightPos);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normalMap);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, heightMap);

		glDrawArrays(GL_TRIANGLES, 0, nVeretx);

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
	else if (key == GLFW_KEY_E)
	{
		if (heightScale > 0.0f)
			heightScale -= 0.0005f;
		else
			heightScale = 0.0f;
	}
	else if (key == GLFW_KEY_Q)
	{
		
		if (heightScale < 1.0f)
			heightScale += 0.0005f;
		else
			heightScale = 1.0f;
	}

}
bool CreateVertexSet(GLfloat** ptrBuffer, int& count)
{
	// positions
	glm::vec3 pos1(-1.0, 1.0, 0.0);
	glm::vec3 pos2(-1.0, -1.0, 0.0);
	glm::vec3 pos3(1.0, -1.0, 0.0);
	glm::vec3 pos4(1.0, 1.0, 0.0);
	// texture coordinates
	glm::vec2 uv1(0.0, 1.0);
	glm::vec2 uv2(0.0, 0.0);
	glm::vec2 uv3(1.0, 0.0);
	glm::vec2 uv4(1.0, 1.0);
	// normal vector
	glm::vec3 nm(0.0, 0.0, 1.0);
	// calculate tangent/bitangent vectors of both triangles
	glm::vec3 tangent1, bitangent1;
	glm::vec3 tangent2, bitangent2;
	// - triangle 1
	glm::vec3 edge1 = pos2 - pos1;
	glm::vec3 edge2 = pos3 - pos1;
	glm::vec2 deltaUV1 = uv2 - uv1;
	glm::vec2 deltaUV2 = uv3 - uv1;

	GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
	tangent1 = glm::normalize(tangent1);

	bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
	bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
	bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
	bitangent1 = glm::normalize(bitangent1);

	// - triangle 2
	edge1 = pos3 - pos1;
	edge2 = pos4 - pos1;
	deltaUV1 = uv3 - uv1;
	deltaUV2 = uv4 - uv1;

	f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
	tangent2 = glm::normalize(tangent2);


	bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
	bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
	bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
	bitangent2 = glm::normalize(bitangent2);

	GLfloat quadVertices[] = {
		// Positions            // normal         // TexCoords  // Tangent                          // Bitangent
		pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
		pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
		pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

		pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
		pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
		pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
	};
	int length = sizeof(quadVertices);
	count = length / sizeof(GLfloat);
	*ptrBuffer = new GLfloat[count];
	memcpy(*ptrBuffer, quadVertices, length);
	return count > 0;
}
