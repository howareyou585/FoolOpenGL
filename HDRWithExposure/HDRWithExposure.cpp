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

	// 注册窗口键盘事件回调函数
	//glfwSetKeyCallback(window, key_callback);
	// 注册窗口鼠标事件回调函数
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	//首先我们要告诉GLFW，它应该隐藏光标，并捕捉(Capture)它。
	//捕捉光标表示的是，如果焦点在你的程序上
	//（译注：即表示你正在操作这个程序，Windows中拥有焦点的程序标题栏通常是有颜色的那个，
	//而失去焦点的程序标题栏则是灰色的），光标应该停留在窗口中（除非程序失去焦点或者退出）。
	//我们可以用一个简单地配置调用来完成：
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//在调用这个函数之后，无论我们怎么去移动鼠标，光标都不会显示了，
	// 它也不会离开窗口。对于FPS摄像机系统来说非常完美。
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
	vecAttrib.emplace_back(vertex_attribute::normal);
	vecAttrib.emplace_back(vertex_attribute::texcoord);
	mapAttrib2Size[vertex_attribute::position] = 3;
	mapAttrib2Size[vertex_attribute::normal] = 3;
	mapAttrib2Size[vertex_attribute::texcoord] = 2;
	vaoBuffer.BuildVAO(cubeVertices2, sizeof(cubeVertices2), nullptr, 
		0, vecAttrib, mapAttrib2Size);
	
	// 创建缓存对象
	GLuint VAOId = vaoBuffer.GetVAO();
	GLuint VBOId = vaoBuffer.GetVBO();
	//加载材质
	GLuint texId  = TextureFromFile("wood.png", "../resources/textures");
	// Light sources
   // - Positions
	vector<glm::vec3> lightPositions;
	lightPositions.emplace_back(glm::vec3(0.0f, 0.0f, 49.5f));
	lightPositions.emplace_back(glm::vec3(-1.4f, -1.9f, 9.0f));
	lightPositions.emplace_back(glm::vec3(0.0f, -1.8f, 4.0f));
	lightPositions.emplace_back(glm::vec3(0.8f, -1.7f, 6.0f));
	// - Colors
	vector<glm::vec3> lightColors;
	lightColors.emplace_back(glm::vec3(200.f, 200.f, 200.f));
	lightColors.emplace_back(glm::vec3(0.1f, 0.0f, 0.0f));
	lightColors.emplace_back(glm::vec3(0.0f, 0.0f, 0.2f));
	lightColors.emplace_back(glm::vec3(0.0f, 0.1f, 0.0f));
	//lightColors.emplace_back(glm::vec3(1.0f, 1.0f, 1.0f));
	//lightColors.emplace_back(glm::vec3(1.0f, 1.0f, 1.0f));
	//lightColors.emplace_back(glm::vec3(1.0f, 1.0f, 1.0f));
	//lightColors.emplace_back(glm::vec3(1.0f, 1.0f, 1.0f));
	//set up floating point framebuffer to render scene;
	//GLuint hdrFBO;
	//glGenFramebuffers(1, &hdrFBO);
	////-Create floating point color buffer
	//GLuint colorBuffer;
	//glGenTextures(1, &colorBuffer);
	//glBindTexture(GL_TEXTURE_2D, colorBuffer);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	////-Create depth buffer(renderBuffer)
	//GLuint rboDepthBuffer;
	//glGenRenderbuffers(1, &rboDepthBuffer);
	//glBindRenderbuffer(GL_RENDERBUFFER, rboDepthBuffer);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WINDOW_WIDTH, WINDOW_HEIGHT);
	//
	////attach buffers
	//glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepthBuffer);
	//if (!glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	//{
	//	return 0;
	//}
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);


	BoundingBox box;
	int nVal = sizeof(cubeVertices2) / sizeof(GLfloat);
	for (int i = 0; i < nVal; i += 8)
	{
		glm::vec3 pnt(cubeVertices2[i], cubeVertices2[i + 1], cubeVertices2[i + 2]);
		box.Merge(pnt);
	}
	
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 25.0));
	model = glm::scale(model, glm::vec3(2.5f, 2.5f, 27.5f));
	BoundingBox totalBoundingBox = box.Transformed(model);

	camera.InitCamera(totalBoundingBox,0.1f);

	// Section2 准备着色器程序
	Shader shader("lighting.vertex", "lighting.frag");
	//Shader hdrShader("hdrExposure.vetex", "hdrExposure.frag");
	shader.use();
	glm::mat4 projectionMatrix = camera.GetProjectionMatrix((float)WINDOW_WIDTH / (float)WINDOW_HEIGHT);
	shader.setMat4("projection", projectionMatrix);
	shader.setMat4("model", model);
	shader.setInt("diffuseMap", 0);
	for (auto i = 0; i < lightPositions.size(); i++)
	{
		shader.setVec3("pointlights[" + std::to_string(i) + "].position", lightPositions[i]);
		shader.setVec3("pointlights[" + std::to_string(i) + "].color", lightColors[i]);
	}
	shader.setBool("inverse_normals", true);
	shader.unUse();
	int nVertex = sizeof(cubeVertices2) / (sizeof(GLfloat) * 8);
	// 开始游戏主循环
	glEnable(GL_DEPTH_TEST);
	glm::vec3 targetPos = totalBoundingBox.GetCenter();

	
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

		// 这里填写场景绘制代码
		glBindVertexArray(VAOId);
		float distance = glm::length(targetPos - camera.Position);
		targetPos = camera.Position + distance * camera.Front;
		glm::mat4 viewMatrix = camera.GetViewMatrix(targetPos);
		shader.use();
		//鼠标移动，镜头方向不变
		shader.setMat4("view", viewMatrix);
	
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texId);

		glDrawArrays(GL_TRIANGLES, 0, nVertex);
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
//void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
//{
//	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
//	{
//		glfwSetWindowShouldClose(window, GL_TRUE); // 关闭窗口
//	}
//}
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