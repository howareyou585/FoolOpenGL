// 引入GLEW库 定义静态链接
#define GLEW_STATIC
#include <GLEW/glew.h>
// 引入GLFW库
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
// 包含着色器加载库
#include "learnopengl/boundingbox.h"
#include "learnopengl/shader.h"
#include "learnopengl/model.h"
#include "learnopengl/filesystem.h"
#include "learnopengl/camera.h"
#include "learnopengl/vertexset.h"
#include "learnopengl/vaobuffer.h"
#include "learnopengl/Maroc.h"
#include <thread>

// 键盘回调函数原型声明
void processInput(GLFWwindow* window, Camera& camera);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);// 定义程序常量
void UpdateInstanceMatrix(vector<glm::mat4>& vecInsMatrix);
void UpdateInstanceColor(vector<glm::vec3>& vecInsColor);
const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;
const int AMOUNT = 10000;
float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间
float lastX = WINDOW_WIDTH / 2.0f;
float lastY = WINDOW_HEIGHT / 2.0f;
bool  bFirstMove = true;
bool  bSwitchColorVBO = false;
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
		"Demo of triangle", NULL, NULL);
	if (!window)
	{
		std::cout << "Error::GLFW could not create winddow!" << std::endl;
		glfwTerminate();
		return -1;
	}
	// 创建的窗口的context指定为当前context
	glfwMakeContextCurrent(window);
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
	GL_CHECK_ERROR; //GLEW有一个历史悠久的bug，调用glewInit()会设置一个GL_INVALID_ENUM的错误标记，
	                //所以第一次调用的glGetError永远会猝不及防地给你返回一个错误代码。
					//如果要修复这个bug，我们建议您在调用glewInit之后立即调用glGetError消除这个标记：
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
	GLuint vaoId = vaoBuffer.GetVAO();
	//准备材质
	GLuint diffuseTextureId = TextureFromFile("container_diffuse.png", "../resources/textures");
	GLuint spacularTexureId = TextureFromFile("container_specular.png", "../resources/textures");
	//设置相机
	BoundingBox box;
	int nVal = sizeof(cubeVertices) / sizeof(GLfloat);
	for (int i = 0; i < nVal; i += 8)
	{
		glm::vec3 pnt(cubeVertices[i], cubeVertices[i + 1], cubeVertices[i + 2]);
		box.Merge(pnt);
	}

	vector<glm::mat4>vecCubeModelMatrix;

	vecCubeModelMatrix.reserve(AMOUNT);

	vector<glm::vec3>vecCubeModelColor;
	vecCubeModelColor.reserve(AMOUNT);
	UpdateInstanceMatrix(vecCubeModelMatrix);
	UpdateInstanceColor(vecCubeModelColor);

	

	BoundingBox totalCubeBoundingBox;
	for (auto& modelMatrix : vecCubeModelMatrix)
	{
		BoundingBox tempBox = box.Transformed(modelMatrix);
		totalCubeBoundingBox.Merge(tempBox);
	}
	camera.InitCamera(totalCubeBoundingBox, 0.8f);
	
	// Section2 准备着色器程序
	Shader sceneShader("scene.vertex", "scene.frag");
	glm::mat4 viewMatrix = camera.GetViewMatrix();
	glm::mat4 projection = camera.GetProjectionMatrix((GLfloat)(WINDOW_WIDTH) / (GLfloat)(WINDOW_HEIGHT));

	sceneShader.use();
	sceneShader.setMat4("view", viewMatrix);
	sceneShader.setMat4("projection", projection);
	sceneShader.unUse();

	GLuint one_query;
	glGenQueries(1, &one_query);
	GL_INPUT_ERROR

	glEnable(GL_DEPTH_TEST);
	// 清除颜色缓冲区 重置为指定颜色
	glClearColor(0.18f, 0.04f, 0.14f, 1.0f);

	// 开始游戏主循环
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame; // 上一帧的时间
		processInput(window, camera);
		glfwPollEvents(); // 处理例如鼠标 键盘等事件
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		

		// 这里填写场景绘制代码
		vaoBuffer.Bind();

		sceneShader.use();
		int nOcclusionCount = 0;
		for (auto i = 0; i < AMOUNT; i++)
		{
			sceneShader.setMat4("model", vecCubeModelMatrix[i]);
			sceneShader.setVec3("color", vecCubeModelColor[i]);
			glBeginQuery(GL_SAMPLES_PASSED, one_query);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glEndQuery(GL_SAMPLES_PASSED);
			GLuint the_result = 0;
			glGetQueryObjectuiv(one_query, GL_QUERY_RESULT, &the_result);
			if (!the_result)
			{
				//cout << "no no no " << endl;
				++nOcclusionCount;
			}
		}
		cout << "nOcclusionCount = " << nOcclusionCount << endl;
		
		sceneShader.unUse();
		glBindVertexArray(0);
		glfwSwapBuffers(window); // 交换缓存
		int span = (int)((glfwGetTime() - currentFrame)*1000);
		char str[100] = {};
		//sprintf(str, "共花费了%dms", span);
		cout << span << endl;

	}
	// 释放资源
	vaoBuffer.DeleteBuffer();
	glDeleteQueries(1, &one_query);
	one_query = 0;
	sceneShader.deleteProgram();
	glfwTerminate();
	return 0;
}

void UpdateInstanceMatrix(vector<glm::mat4>& vecInsMatrix)
{
	vector<glm::mat4>().swap(vecInsMatrix);
	srand(static_cast<unsigned int>(glfwGetTime())); // initialize random seed
	float radius = 5.0;
	float offset = 0.25f;
	for (unsigned int i = 0; i < AMOUNT; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		// 1. translation: displace along circle with 'radius' in range [-offset, offset]
		float angle = (float)i / (float)AMOUNT * 360.0f;
		float displacement = (rand() % (int)(2 * offset * AMOUNT)) / (float)AMOUNT - offset;
		float x = sin(angle) * radius + displacement;
		displacement = (rand() % (int)(2 * offset * AMOUNT)) / (float)AMOUNT - offset;
		float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
		displacement = (rand() % (int)(2 * offset * AMOUNT)) / (float)AMOUNT - offset;
		float z = cos(angle) * radius + displacement;
		model = glm::translate(model, glm::vec3(x, y, z));

		// 2. scale: Scale between 0.05 and 0.25f
		float scale = static_cast<float>((rand() % 20) / 100.0 + 0.05);
		model = glm::scale(model, glm::vec3(scale));

		// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
		float rotAngle = static_cast<float>((rand() % 360));
		model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));
		vecInsMatrix.emplace_back(model);
	}
}
void UpdateInstanceColor(vector<glm::vec3>& vecColor)
{
	for (unsigned int i = 0; i < AMOUNT; i++)
	{
		// Also calculate random color
		GLfloat rColor = ((rand() % AMOUNT) / 200.0f) + 0.5; // Between 0.5 and 1.0
		GLfloat gColor = ((rand() % AMOUNT) / 200.0f) + 0.5; // Between 0.5 and 1.0
		GLfloat bColor = ((rand() % AMOUNT) / 200.0f) + 0.5; // Between 0.5 and 1.0
		vecColor.push_back(glm::vec3(rColor, gColor, bColor));
	}
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE); // 关闭窗口
	}
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		bSwitchColorVBO = !bSwitchColorVBO;
	}
}
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