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
#include "learnopengl/boundingbox.h"
#include "learnopengl/model.h"
#include "learnopengl/camera.h"
// 键盘回调函数原型声明
//void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow* window, Camera& camera);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

//
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
	std::cout << "Start OpenGL core profile version 4.5" << std::endl;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// 创建窗口
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
		"Demo of blend", NULL, NULL);
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

	//创建透明GPU 缓存
	VAOBuffer vaoTransparentBuffer;
	vector<vertex_attribute> vecAttrib;
	map<vertex_attribute, int> mapAttrib2Size;
	vecAttrib.emplace_back(vertex_attribute::position);
	vecAttrib.emplace_back(vertex_attribute::texcoord);
	mapAttrib2Size[vertex_attribute::position] = 3;
	mapAttrib2Size[vertex_attribute::texcoord] = 2;
	float transparentVertices[] = {
		// positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
		1.0f,  0.5f,  0.0f,  1.0f,  0.0f
	};
	vaoTransparentBuffer.BuildVAO(transparentVertices, sizeof(transparentVertices), nullptr,
		0, vecAttrib, mapAttrib2Size);

	// 创建缓存对象
	GLuint transparentVAOId = vaoTransparentBuffer.GetVAO();
	GLuint transparentVBOId = vaoTransparentBuffer.GetVBO();
	//加载材质
	
	GLuint texWindowId = TextureFromFile("window.png", "../resources/textures");
	// world space positions of our cubes
	glm::vec3 vegetation[] = {
		glm::vec3(-1.5f, 0.0f, -0.48f),
		glm::vec3(1.5f, 0.0f, 0.51f),
		glm::vec3(0.0f, 0.0f, 0.7f),
		glm::vec3(-0.3f, 0.0f, -2.3f),
		glm::vec3(0.5f, 0.0f, -0.6f)
	};
	BoundingBox box;
	int nVal = sizeof(transparentVertices) / sizeof(GLfloat);
	for (int i = 0; i < nVal; i += 5)
	{
		glm::vec3 pnt(transparentVertices[i], transparentVertices[i + 1], transparentVertices[i + 2]);
		box.Merge(pnt);
	}
	BoundingBox totalBoundingBox;
	
	//vector<glm::mat4> vecModelMatrix;
	map<glm::mat4*,BoundingBox> mapModelMtx2BoundingBox;
	int nModelMatrix = sizeof(vegetation)/sizeof(glm::vec3) ;
	//vecModelMatrix.reserve(nModelMatrix);
	for (int i = 0; i < nModelMatrix; i++)
	{
		glm::mat4* ptrModel = new glm::mat4(1.0f);
		*ptrModel = glm::translate(*ptrModel, vegetation[i]);
		BoundingBox tmpBoundingBox = box.Transformed(*ptrModel);
		totalBoundingBox.Merge(tmpBoundingBox);
		//vecModelMatrix.emplace_back(model);
		mapModelMtx2BoundingBox[ptrModel] = tmpBoundingBox;
	}
	//float raduis = totalBoundingBox.GetLength()*0.8f;

	camera.InitCamera(totalBoundingBox, 0.8f);
	float radius = totalBoundingBox.GetLength() * 0.8f;

	// Section2 准备着色器程序
	Shader shader("BlendDual.vertex", "BlendDual.frag");
	shader.use();
	shader.setInt("s_texture", 0);
	shader.unUse();
	int nVertex = sizeof(transparentVertices) / (sizeof(GLuint) * 5);
	// 开始游戏主循环
	glEnable(GL_DEPTH_TEST); //一定要开启深度测试
	glEnable(GL_BLEND);
	//glBlendFunc(GL_ONE, GL_SRC1_COLOR);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendColor(0.2f, 0.5f, 0.7f, 0.5f);
	//glCullFace(GL_BACK);
	glm::vec3 targetPos = totalBoundingBox.GetCenter();
	float distance = glm::length(targetPos - camera.Position);
	
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame; // 上一帧的时间
		processInput(window, camera);
		glfwPollEvents(); // 处理例如鼠标 键盘等事件

		// 清除颜色缓冲区 重置为指定颜色
		glClearColor(0.18f, 0.04f, 0.14f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDepthMask(GL_FALSE);//一定要加上这句
		// 这里填写场景绘制代码
		glBindVertexArray(transparentVAOId);
		shader.use();
		//鼠标移动，镜头方向不变
		targetPos = camera.Position + distance * camera.Front;
		glm::mat4 viewMatrix = camera.GetViewMatrix(targetPos);
		
		shader.setMat4("view", viewMatrix);
		glm::mat4 projectionMatrix = camera.GetProjectionMatrix((float)WINDOW_WIDTH / (float)WINDOW_HEIGHT);
		shader.setMat4("projection", projectionMatrix);
		map<float, glm::mat4> mapDistance2Matrix;
		for (auto it = mapModelMtx2BoundingBox.begin(); it != mapModelMtx2BoundingBox.end(); it++)
		{
			auto& bbox = it->second;
			auto center = bbox.GetCenter();
			auto dist = glm::length(center - camera.Position);
			mapDistance2Matrix[dist] = *(it->first);
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texWindowId);
		//先绘制远处的透明模型，在绘制近处的透明模型
		for (auto rit = mapDistance2Matrix.begin(); rit!=mapDistance2Matrix.end();rit++)
		{
			shader.setMat4("model", rit->second);
			glDrawArrays(GL_TRIANGLES, 0, nVertex);
			//glDrawArrays(GL_LINES, 0, nVertex);
		}
		shader.unUse();
		glBindVertexArray(0);
		glUseProgram(0);

		glfwSwapBuffers(window); // 交换缓存
	}
	// 释放资源
	glDeleteVertexArrays(1, &transparentVAOId);
	glDeleteBuffers(1, &transparentVBOId);
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
void processInput(GLFWwindow* ptrWindow, Camera& camera)
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