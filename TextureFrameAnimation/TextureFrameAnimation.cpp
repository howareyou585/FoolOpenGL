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
#include "learnopengl/vaoBuffer.h"
#include "learnopengl/vertexset.h"
#include "learnopengl/boundingbox.h"
#include "learnopengl/model.h"
#include "learnopengl/camera.h"
#include <thread>
#include <chrono>
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
	float tempVertices[] = {
		10.f,  10.f, 0.f,  0.f,1.f,
		410.f, 10.f, 0.f,  1.f,1.f,
		10.f,  410.f,0.f,  0.f,  0.f,
		410.f, 410.f,0.f,  1.f,0.
	};
	int nVal = sizeof(tempVertices) / sizeof(GLfloat);
	float* ptrVerticesBak = new float[nVal];
	memcpy(ptrVerticesBak, tempVertices, sizeof(tempVertices));
	GLuint tempIndexes[] = { 0,1,2,1,2,3};
	VAOBuffer vaoBuffer;
	vector<vertex_attribute> vecAttrib;
	map<vertex_attribute, int> mapAttrib2Size;
	vecAttrib.emplace_back(vertex_attribute::position);
	vecAttrib.emplace_back(vertex_attribute::texcoord);
	mapAttrib2Size[vertex_attribute::position] = 3;
	mapAttrib2Size[vertex_attribute::texcoord] = 2;
	vaoBuffer.BuildVAO(tempVertices, sizeof(tempVertices), tempIndexes,
		sizeof(tempIndexes), vecAttrib, mapAttrib2Size);

	// 创建缓存对象
	GLuint VAOId = vaoBuffer.GetVAO();
	GLuint VBOId = vaoBuffer.GetVBO();
	//加载材质
	//
	GLuint texId = TextureFromFile("flower.jpg", "../resources/textures",false, GL_CLAMP_TO_EDGE);
	//GLuint texId = TextureFromFile("explore.jpg", "../resources/textures");

	BoundingBox box;
	
	for (int i = 0; i < nVal; i += 5)
	{
		glm::vec3 pnt(tempVertices[i], tempVertices[i + 1], tempVertices[i + 2]);
		box.Merge(pnt);
	}
	
	
	camera.InitCamera(box, 1.8f);
	float radius = box.GetLength() * 1.8f;

	// Section2 准备着色器程序
	Shader shader("TextureFrameAnimation.vertex", "TextureFrameAnimation.frag");
	shader.use();
	shader.setInt("sampler2d", 0);
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 projection = camera.GetProjectionMatrix((float)WINDOW_WIDTH/(float)(WINDOW_HEIGHT));
	glm::mat4 model(1.0f);
	model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
	shader.setMat4("model", model);
	shader.setMat4("view", view);
	shader.setMat4("projection", projection);
	shader.unUse();
	int nVertex = sizeof(tempVertices) / (sizeof(GLuint) * 5);
	// 开始游戏主循环
	glEnable(GL_DEPTH_TEST);
	//glCullFace(GL_BACK);
	glm::vec3 targetPos = box.GetCenter();
	float distance = glm::length(targetPos - camera.Position);
	float angle = 0.0f;
	
	lastFrame = 0;
	int row = 3; //纹理帧动画 有3行 
	int col = 3; //纹理帧动画 有3列 
	float scale = 1.0 / row;
	glm::vec2 uvSize(scale, scale);
	//int frame = 0;
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		//超过9的话，从头（0）开始
		int frame = ((int)(currentFrame*9)) % 9; // 此处一定要注意：括号里*9后，再强制转为int,
												 // 如果没有（）则取模一定为0
	
		//当前行
		int curRow = frame / row;
		//当前列
		int curCol = frame % col;

		
		for (int i = 0; i < nVal; i+=5)
		{
			
			tempVertices[i + 3] *= scale;
			tempVertices[i + 4] *= scale;

			tempVertices[i + 3] += scale * curCol;
			tempVertices[i + 4] += scale * curRow;
			float x = tempVertices[i + 0];
			float y = tempVertices[i + 1];
			float z = tempVertices[i + 2];
			float u = tempVertices[i + 3];
			float v = tempVertices[i + 4];
			

			string strXYZ = "(" + to_string(x)+ ","+ to_string(y) + "," + to_string(z)+ ")";
			string strUV  = "(" + to_string(u)+ "," + to_string(v)+ ")";
			cout << strXYZ << "  " << strUV << endl;
		}
		cout << "currentFrame:" << currentFrame << ", " << "frame:" << frame << endl;
		cout << endl;
		glBindVertexArray(VAOId);
		glBindBuffer(GL_ARRAY_BUFFER, VBOId);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(tempVertices), tempVertices);
		
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		memcpy(tempVertices, ptrVerticesBak, sizeof(tempVertices));
		
		deltaTime = currentFrame - lastFrame; //渲染一帧用时
		//cout << to_string(deltaTime) << endl;
		lastFrame = currentFrame; // 上一帧的时间
		processInput(window, camera);
		glfwPollEvents(); // 处理例如鼠标 键盘等事件

		// 清除颜色缓冲区 重置为指定颜色
		glClearColor(0.18f, 0.04f, 0.14f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 这里填写场景绘制代码
		glBindVertexArray(VAOId);
		
		shader.use();
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, texId);
		
		//这里一定要绑定EBO，否则执行glDrawElements时挂机
		vaoBuffer.BindEBO();
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		//glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		shader.unUse();
		glBindVertexArray(0);
		glUseProgram(0);

		glfwSwapBuffers(window); // 交换缓存
		//std::this_thread::sleep_for(std::chrono::seconds(1));
		
	}
	if (ptrVerticesBak)
	{
		delete[] ptrVerticesBak;
		ptrVerticesBak = nullptr;
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