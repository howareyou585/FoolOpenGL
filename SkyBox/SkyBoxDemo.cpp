// 引入GLEW库 定义静态链接
#define GLEW_STATIC
#include <GLEW/glew.h>
// 引入GLFW库
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtc/matrix_transform.hpp>
// 包含着色器加载库
#include "learnopengl/shader.h"
#include "learnopengl/camera.h"
#include "learnopengl/model.h"
#include "learnopengl/vertexset.h"
#include "learnopengl/vaobuffer.h"
#include "learnopengl/skybox.h"
// 键盘回调函数原型声明
//void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow *window, Camera & camera);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
// 定义程序常量
const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;
float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间
float lastX = WINDOW_WIDTH / 2.0f;
float lastY = WINDOW_HEIGHT / 2.0f;
bool  bFirstMove = true;
Camera camera;

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
	// 注册窗口鼠标事件回调函数
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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

	//创建cube VAO
	vector<vertex_attribute> vecAttrib;
	map<vertex_attribute, int> mapAttrib2Size;
	vecAttrib.emplace_back(vertex_attribute::position);
	vecAttrib.emplace_back(vertex_attribute::texcoord);
	mapAttrib2Size[vertex_attribute::position] = 3;
	mapAttrib2Size[vertex_attribute::texcoord] = 2;
	VAOBuffer cubeVaoBuffer;
	cubeVaoBuffer.BuildVAO(cubeVertices3, sizeof(cubeVertices3), nullptr, 0, vecAttrib, mapAttrib2Size);
	// 创建缓存对象
	GLuint cubeVAOId = cubeVaoBuffer.GetVAO();
	GLuint cubeVBOId = cubeVaoBuffer.GetVBO();
	unsigned int woodTextureId = TextureFromFile("wood.png", "../resources/textures");
	//创建CubeMap
	std::vector<string> vecFilePath;
	vecFilePath.push_back("../resources/skyboxes/sky/sky_rt.jpg");
	vecFilePath.push_back("../resources/skyboxes/sky/sky_lf.jpg");
	vecFilePath.push_back("../resources/skyboxes/sky/sky_up.jpg");
	vecFilePath.push_back("../resources/skyboxes/sky/sky_dn.jpg");
	vecFilePath.push_back("../resources/skyboxes/sky/sky_bk.jpg");
	vecFilePath.push_back("../resources/skyboxes/sky/sky_ft.jpg");
	SkyBox skyBox(vecFilePath);
	unsigned int cubeMapId = skyBox.LoadCubeMap();

	//创建天空盒的VAO
	VAOBuffer skyboxVAOBuffer;
	vecAttrib.clear();
	mapAttrib2Size.clear();
	vecAttrib.emplace_back(vertex_attribute::position);
	mapAttrib2Size[vertex_attribute::position] = 3;
	skyboxVAOBuffer.BuildVAO(skyBox.m_vertices,
		sizeof(skyBox.m_vertices), nullptr, 0,
		vecAttrib, mapAttrib2Size);
	GLuint skyBoxVAOId = skyboxVAOBuffer.GetVAO();
	GLuint skyBoxVBOId = skyboxVAOBuffer.GetVBO();

	BoundingBox bbox;
	int num = sizeof(cubeVertices3) /  sizeof(GLfloat);
	for (auto i = 0; i < num; i+=5)
	{
		glm::vec3 pnt(cubeVertices3[i], cubeVertices3[i + 1], cubeVertices3[i + 2]);
		bbox.Merge(pnt);
	}
	auto length = bbox.GetLength()*2.0f;
	// Section2 准备着色器程序
	Shader skyboxShader("skybox.vertex","skybox.frag");
	Shader cubeShader("scene.vertex", "scene.frag");

	glEnable(GL_DEPTH_TEST);//深度测试
	//glEnable(GL_CULL_FACE);//剔除面
	// 开始游戏主循环
	skyboxShader.use();
	//skyboxShader.setMat4("projection", projection);
	skyboxShader.setInt("skybox", 0);
	skyboxShader.unUse();

	cubeShader.use();
	//cubeShader.setMat4("projection", projection);
	cubeShader.setInt("txt", 0);
	cubeShader.unUse();
	glm::mat4 modelMatrix(1.0f);
	BoundingBox tmpbbox = bbox.Transformed(modelMatrix);
	camera.Position = tmpbbox.GetCenter() - camera.Front * length;
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame; // 上一帧的时间
		processInput(window, camera);
		glfwPollEvents(); // 处理例如鼠标 键盘等事件

		// 清除颜色缓冲区 重置为指定颜色
		glClearColor(0.18f, 0.04f, 0.14f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		
		skyboxVAOBuffer.Bind();
		skyboxShader.use();
		glDepthMask(false);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapId);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 viewMatrix = glm::mat4(glm::mat3(view));
		skyboxShader.setMat4("view", viewMatrix);
		
		glm::mat4 projection = camera.GetProjectionMatrix((GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT,ZOOM);
		skyboxShader.setMat4("projection", projection);

		glDrawArrays(GL_TRIANGLES, 0, 36);
		skyboxShader.unUse();
		skyboxVAOBuffer.UnBind();

		cubeVaoBuffer.Bind();
		glDepthMask(true);

		cubeShader.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, woodTextureId);
		
		cubeShader.setMat4("model", modelMatrix);
		cubeShader.setMat4("view",  view);
		projection = camera.GetProjectionMatrix((GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT);
		cubeShader.setMat4("projection", projection);
	
		glDrawArrays(GL_TRIANGLES, 0, 36);
		cubeShader.unUse();
		cubeVaoBuffer.UnBind();
		glBindVertexArray(0);
		glUseProgram(0);

		glfwSwapBuffers(window); // 交换缓存
	}
	// 释放资源
	glDeleteVertexArrays(1, &cubeVAOId);
	glDeleteBuffers(1, &cubeVBOId);
	glfwTerminate();
	return 0;
}
void processInput(GLFWwindow *ptrWindow, Camera & camera)
{
	if (!ptrWindow)
	{
		return;
	}
	Camera_Movement direction;
	bool bMove = false;
	if (glfwGetKey(ptrWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(ptrWindow, true);

	}
	else if (glfwGetKey(ptrWindow, GLFW_KEY_W) == GLFW_PRESS)
	{
		direction = Camera_Movement::FORWARD;
		bMove = true;
		//camera.ProcessKeyboard()
	}
	else if (glfwGetKey(ptrWindow, GLFW_KEY_S) == GLFW_PRESS)
	{
		direction = Camera_Movement::BACKWARD;
		bMove = true;
		//camera.ProcessKeyboard()
	}
	else if (glfwGetKey(ptrWindow, GLFW_KEY_A) == GLFW_PRESS)
	{
		direction = Camera_Movement::LEFT;
		bMove = true;
		//camera.ProcessKeyboard()
	}
	else if (glfwGetKey(ptrWindow, GLFW_KEY_D) == GLFW_PRESS)
	{
		direction = Camera_Movement::RIGHT;
		bMove = true;
		//camera.ProcessKeyboard()
	}
	if (bMove)
	{
		camera.ProcessKeyboard(direction, deltaTime);
	}
}
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);
	if (bFirstMove)
	{
		lastX = xpos;
		lastY = ypos;
		bFirstMove = false;
	}

	float offsetX = xpos - lastX;
	float offsetY = lastY - ypos; // y 坐标往下越来越大。
	//std::cout << "lastY="<<lastY <<",ypos =" <<ypos<< ",offsetY =" <<offsetY << endl;
	//std::cout << "offsetX=" << offsetX << ",offsetY=" << offsetY << std::endl;
	lastX = xpos;
	lastY = ypos;
	camera.ProcessMouseMovement(offsetX, offsetY);
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	//std::cout << "xoffset=" << xoffset << ",yoffset=" << yoffset << std::endl;
	camera.ProcessMouseScroll(yoffset);
}