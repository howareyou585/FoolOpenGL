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
#include "learnopengl/lightManager.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
// 键盘回调函数原型声明
void processInput(GLFWwindow* window, Camera& camera);
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
void RenderCube(const Shader& shader, vector<glm::mat4>& vecModelMatrix);
void RenderLight(const Shader& shader, glm::mat4&modelMatrix);
void RenderPlane(const Shader& shader);
void RenderQuad(const Shader& shader);
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
	// 注册窗口鼠标事件回调函数
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	//首先我们要告诉GLFW，它应该隐藏光标，并捕捉(Capture)它。
	//捕捉光标表示的是，如果焦点在你的程序上
	//（译注：即表示你正在操作这个程序，Windows中拥有焦点的程序标题栏通常是有颜色的那个，
	//而失去焦点的程序标题栏则是灰色的），光标应该停留在窗口中（除非程序失去焦点或者退出）。
	//我们可以用一个简单地配置调用来完成：
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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
#pragma region imgui初始化
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();//主题颜色
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");
#pragma endregion
	// 创建cube缓存对象
	vector<vertex_attribute> vecVertexAttrib;
	map< vertex_attribute, int> mapVertexAttrib2Num;
	vecVertexAttrib.push_back(vertex_attribute::position);
	vecVertexAttrib.push_back(vertex_attribute::normal);
	vecVertexAttrib.push_back(vertex_attribute::texcoord);
	mapVertexAttrib2Num[vertex_attribute::position] = 3;
	mapVertexAttrib2Num[vertex_attribute::normal] = 3;
	mapVertexAttrib2Num[vertex_attribute::texcoord] = 2;
	VAOBuffer cubeVaoBuffer;
	cubeVaoBuffer.BuildVAO(cubeVertices2, sizeof(cubeVertices2),
		nullptr, 0, vecVertexAttrib, mapVertexAttrib2Num);

	VAOBuffer planeVaoBuffer;
	planeVaoBuffer.BuildVAO(planeVertices, sizeof(planeVertices),
		nullptr, 0,
		vecVertexAttrib, mapVertexAttrib2Num);

	GLuint cubeVAOId = cubeVaoBuffer.GetVAO();
	GLuint planeVAOId = planeVaoBuffer.GetVAO();


	//准备材质
	GLuint woodTexId = TextureFromFile("wood.png", "../resources/textures");
	//configure depth map FBO
	const unsigned int SHADOW_WIDTH = 1024;
	const unsigned int SHADOW_HEIGTH = 1024;
	//创建深度贴图FBO
	GLuint depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	GLuint depthMapTextureID;
	glGenTextures(1, &depthMapTextureID);
	glBindTexture(GL_TEXTURE_2D, depthMapTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGTH,
		0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTextureID, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//设置相机
	BoundingBox box;
	int nVal = sizeof(cubeVertices2) / sizeof(GLfloat);
	for (int i = 0; i < nVal; i += 8)
	{
		glm::vec3 pnt(cubeVertices2[i], cubeVertices2[i + 1], cubeVertices2[i + 2]);
		box.Merge(pnt);
	}
	vector<glm::mat4> vecModelMatrix;
	//instance1
	glm::mat4 model(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
	model = glm::scale(model, glm::vec3(0.5f));
	vecModelMatrix.emplace_back(model);
	//instance2 
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
	model = glm::scale(model, glm::vec3(0.5f));
	vecModelMatrix.emplace_back(model);
	//instance3
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
	model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	model = glm::scale(model, glm::vec3(0.25));
	vecModelMatrix.emplace_back(model);
	BoundingBox totalBox;
	for (int i = 0; i < vecModelMatrix.size(); i++)
	{
		totalBox.Merge(box.Transformed(vecModelMatrix[i]));
	}
	// Section2 准备着色器程序
	Shader shadowMappingShader("shadow_mapping.vertex", "shadow_mapping.frag");
	Shader simpleDepthShader("shadow_mapping_depth.vertex", "shadow_mapping_depth.frag");
	Shader lightShader("light_cube.vertex", "light_cube.frag");
	//Shader quadShader("quad.vertex", "quad.frag");
	glm::vec3 center = totalBox.GetCenter();
	float boxLength = totalBox.GetLength();
	glm::vec3 position = center + (boxLength * 2.5f) * glm::vec3(0, 0, 1.0f);
	//glm::vec3 position(0.f, 0.f, 3.0f);
	camera.SetPosition (position);
	/*glm::vec3 lightPos = center + (boxLength)*glm::vec3(0, 0, 1.0f);
	lightPos.y += boxLength * 0.5f;
	lightPos.x -= boxLength * 0.5f;*/
	glm::vec3 lightPos = glm::vec3(-2.0f, 4.0f, -1.0f);
	
	bool bDirection = false;
	float bais = 0.1f;
	int pcfvalue = 0;
	float nearPlane = 1.f;
	//float farPlane = 7.5f;
	float farPlane = 200.0f; 
	float val = 10.f;
	glm::mat4 lightProjection = glm::ortho(-val, val, -val, val, nearPlane, farPlane);
	//glm::mat4 lightProjection = glm::ortho(camera.Zoom, (GLfloat)WINDOW_WIDTH / WINDOW_HEIGHT, 0.01f, 10000.0f);
	glm::mat4 cameraProjection = glm::perspective(camera.Zoom, (GLfloat)WINDOW_WIDTH / WINDOW_HEIGHT, 0.01f, 1000.0f);
	

	shadowMappingShader.use();
	//设置相机参数
	
	shadowMappingShader.setMat4("projection", cameraProjection);
	shadowMappingShader.setVec3("eyePos", camera.Position);
	
	//设置平行灯光参数
	glm::vec3 ambient(0.09f, 0.09f, 0.09f);
	glm::vec3 diffuse(0.3f, 0.3f, 0.3f);
	glm::vec3 spacular(0.3f, 0.3f, 0.3f);
	
	glm::vec3 lightDir = glm::normalize(center - lightPos);
	Directionlight dirLight(ambient, diffuse, spacular, lightDir);
	dirLight.SetLightUniformParam(shadowMappingShader, "light.");
	//设置材质参数
	
	shadowMappingShader.setInt("material.diffuseTexture", 0);
	shadowMappingShader.setInt("shadowMap", 1);
	shadowMappingShader.setFloat("material.shiness", 256.0f);
	shadowMappingShader.setVec3("light.direction", lightDir);
	shadowMappingShader.unUse();

	lightShader.use();
	
	lightShader.setMat4("projection", cameraProjection);
	lightShader.unUse();
	/*quadShader.use();
	quadShader.setInt("depthMap", 0);
	quadShader.unUse();*/
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	// 开始游戏主循环
	while (!glfwWindowShouldClose(window))
	{
		processInput(window, camera);
		glfwPollEvents(); // 处理例如鼠标 键盘等事件
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame; // 上一帧的时间
		// 清除颜色缓冲区 重置为指定颜色
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		//用户定义的深度framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGTH);
		// 这里填写场景绘制代码
		//glCullFace(GL_FRONT); //只对封闭的模型有效果，开放的模型没有效果
		planeVaoBuffer.Bind();
		simpleDepthShader.use();

		glm::mat4 lightView = glm::lookAt(lightPos,
			glm::vec3(0.f, 0.f, 0.f),
			glm::vec3(0.f, 1.0f, 0.0f));
		glm::mat4 lightSpaceMatrix = lightProjection * lightView;
		simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, woodTexId);
		//绘制地板
		RenderPlane(simpleDepthShader);
		planeVaoBuffer.UnBind();

		cubeVaoBuffer.Bind();
		RenderCube(simpleDepthShader, vecModelMatrix);
		cubeVaoBuffer.UnBind();
		simpleDepthShader.unUse();
		//glCullFace(GL_BACK); //只对封闭的模型有效果，开放的模型没有效果
		//系统默认的framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// reset viewport
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

		shadowMappingShader.use();
		shadowMappingShader.setMat4("view", camera.GetViewMatrix());
		shadowMappingShader.setVec3("light.position", lightPos);

		shadowMappingShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		shadowMappingShader.setBool("bDirection", bDirection);
		shadowMappingShader.setFloat("bais", bais/100);
		shadowMappingShader.setInt("pcf", pcfvalue);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, woodTexId);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMapTextureID);
		
		planeVaoBuffer.Bind();
		RenderPlane(shadowMappingShader);
		planeVaoBuffer.UnBind();

		cubeVaoBuffer.Bind();
		RenderCube(shadowMappingShader, vecModelMatrix);
		shadowMappingShader.unUse();

		lightShader.use();
		lightShader.setMat4("view", camera.GetViewMatrix());
		glm::mat4 lightModelMatrix(1.0);
		lightModelMatrix = glm::scale(lightModelMatrix, glm::vec3(0.15f, 0.15f, 0.15));
		lightModelMatrix = glm::translate(lightModelMatrix, glm::vec3(lightPos));
		RenderLight(lightShader,lightModelMatrix);
		lightShader.unUse();
#pragma region create element
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::Begin("Hello Gamma");
		
		
		string strLightPos = "light position ";
		ImGui::DragFloat3(strLightPos.c_str(), (float*)(&lightPos.x), 1.0f, -20.0f, 20.0f);
		ImGui::Checkbox(u8"是否为平行光", &bDirection);
		ImGui::DragFloat("bais", &bais, 0.05, 0, 1);
		ImGui::DragInt("PCF", &pcfvalue, 1, 0, 5);

		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#pragma endregion
		cubeVaoBuffer.UnBind();
		
		glfwSwapBuffers(window); // 交换缓存
	}
	// 释放资源
	glDeleteVertexArrays(1, &cubeVAOId);
	GLuint cubeVboId = cubeVaoBuffer.GetVBO();
	glDeleteBuffers(1, &cubeVboId);

	glDeleteVertexArrays(1, &planeVAOId);
	GLuint planeVBOId = planeVaoBuffer.GetVBO();
	glDeleteBuffers(1, &planeVBOId);

#pragma region Cleanup imgui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
#pragma endregion
	glfwTerminate();
	return 0;
}

void RenderCube(const Shader& shader, vector<glm::mat4>& vecModelMatrix)
{
	for (int i = 0; i < vecModelMatrix.size(); i++)
	{
		shader.setMat4("model", vecModelMatrix[i]);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}
void RenderLight(const Shader& shader, glm::mat4&modelMatrix)
{
	shader.setMat4("model", modelMatrix);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}
void RenderPlane(const Shader& shader)
{
	glm::mat4 model = glm::mat4(1.f);
	shader.setMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
void RenderQuad(const Shader& shader)
{

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