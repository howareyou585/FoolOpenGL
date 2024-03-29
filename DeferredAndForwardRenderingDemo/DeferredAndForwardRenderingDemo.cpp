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
#include "learnopengl/fboBuffer.h"
#include "learnopengl/vaobuffer.h"
#include "learnopengl/vertexset.h"
#include "learnopengl/light.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// 键盘回调函数原型声明
void processInput(GLFWwindow* window, Camera& camera);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);// 定义程序常量
void UpdateInstanceMatrix(vector<glm::mat4>& vecInsMatrix);
const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;
float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间
float lastX = WINDOW_WIDTH / 2.0f;
float lastY = WINDOW_HEIGHT / 2.0f;
bool  bFirstMove = true;
Camera camera;

GLuint quadVAOId = 0, quadVBOId = 0;
GLuint cubeVAOId = 0, cubeVBOId = 0;
bool PrepareQuadBuffer();
bool PrepareCubeBuffer();
void RenderQuad();
float yOffset = 0.f;
float zOffset = 0.f;
bool volumes = true;
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
#pragma region imgui初始化
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();//主题颜色
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");
#pragma endregion
	//glfwSetCursorPosCallback(window, mouse_callback);
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

	// 设置视口参数
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	PrepareQuadBuffer();
	PrepareCubeBuffer();
	
	// Section1 准备顶点数据
	// 指定顶点属性数据 顶点位置
	Model nanosuit(FileSystem::getPath("Model/backpack/backpack.obj"));
#pragma region 场景中模型的位置

	std::vector<glm::vec3> objectPositions;
	objectPositions.push_back(glm::vec3(-3.0, -0.5, -3.0));
	objectPositions.push_back(glm::vec3(0.0, -0.5, -3.0));
	objectPositions.push_back(glm::vec3(3.0, -0.5, -3.0));
	objectPositions.push_back(glm::vec3(-3.0, -0.5, 0.0));
	objectPositions.push_back(glm::vec3(0.0, -0.5, 0.0));
	objectPositions.push_back(glm::vec3(3.0, -0.5, 0.0));
	objectPositions.push_back(glm::vec3(-3.0, -0.5, 3.0));
	objectPositions.push_back(glm::vec3(0.0, -0.5, 3.0));
	objectPositions.push_back(glm::vec3(3.0, -0.5, 3.0));
#pragma endregion
#pragma region 根据模型设置相机的位置
	BoundingBox sceneBoundingBox;
	BoundingBox& box = nanosuit.GetBoundingBox();
	glm::mat4 modelMatrix(1.0f);
	for (auto& item : objectPositions)
	{
		glm::mat4 tempModelMatrix = glm::translate(modelMatrix, item);
		BoundingBox tempBoundingBox = box.Transformed(tempModelMatrix);
		sceneBoundingBox.Merge(tempBoundingBox);
	}
	camera.InitCamera(sceneBoundingBox, 0.8f);
	auto length = sceneBoundingBox.GetLength() * 0.8f;
#pragma endregion
	// Section2 准备着色器程序
	Shader shaderGeometryPass("g_buffer.vertex", "g_buffer.frag");
	Shader shaderLightingPass("deferred_shading.vertex", "deferred_shading.frag");
	Shader shaderLightBox("light_box.vertex", "light_box.frag");

#pragma region 设置多个灯光参数
	const GLuint NR_LIGHTS = 32;
	std::vector<glm::vec3> lightPositions;
	std::vector<glm::vec3> lightColors;
	srand(13);
	for (GLuint i = 0; i < NR_LIGHTS; i++)
	{
		// Calculate slightly random offsets
		GLfloat xPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
		GLfloat yPos = ((rand() % 100) / 100.0) * 6.0 - 4.0;
		GLfloat zPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
		lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
		// Also calculate random color
		GLfloat rColor = ((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0
		GLfloat gColor = ((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0
		GLfloat bColor = ((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0
		lightColors.push_back(glm::vec3(rColor, gColor, bColor));
	}
	float constant = 1.0f;
	float linear = 0.7;
	float quadratic = 1.8f;
#pragma endregion
#pragma region  创建G-Buffer
	GLuint gBufferFrameBufferId;
	glGenFramebuffers(1, &gBufferFrameBufferId);
	glBindFramebuffer(GL_FRAMEBUFFER, gBufferFrameBufferId);
	//positon color buffer
	GLuint gPositionTextureId;
	glGenTextures(1, &gPositionTextureId);
	glBindTexture(GL_TEXTURE_2D, gPositionTextureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);*/
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPositionTextureId, 0);
	
	//normal color buffer
	GLuint gNormalTextureId;
	glGenTextures(1, &gNormalTextureId);
	glBindTexture(GL_TEXTURE_2D, gNormalTextureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);*/
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormalTextureId, 0);
	// color + shiness color buffer
	GLuint gAlbedoSpecTextureId;
	glGenTextures(1, &gAlbedoSpecTextureId);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpecTextureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpecTextureId, 0);

	GLenum attachments[3] = { GL_COLOR_ATTACHMENT0 ,GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);

	GLuint rboDepthId;
	glGenRenderbuffers(1, &rboDepthId);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepthId);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WINDOW_WIDTH, WINDOW_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepthId);
	// - Finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Framebuffer not complete!" << std::endl;
		return -1;
	}
#pragma endregion
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.18f, 0.04f, 0.14f, 1.0f);
	shaderLightingPass.use();
	shaderLightingPass.setInt("gPosition", 0);
	shaderLightingPass.setInt("gNormal", 1);
	shaderLightingPass.setInt("gAlbedoSpec", 2);
	shaderLightingPass.unUse();
	// 开始游戏主循环
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame; // 上一帧的时间
		processInput(window, camera);
		glfwPollEvents(); // 处理例如鼠标 键盘等事件
		glBindFramebuffer(GL_FRAMEBUFFER, gBufferFrameBufferId);
		// 清除颜色缓冲区 重置为指定颜色
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shaderGeometryPass.use();
		
		glm::mat4 viewMatrix = camera.GetViewMatrix();
		shaderGeometryPass.setMat4("viewMatrix", viewMatrix);
		glm::mat4 projection = camera.GetProjectionMatrix((GLfloat)(WINDOW_WIDTH) / (GLfloat)(WINDOW_HEIGHT));
		shaderGeometryPass.setMat4("projectionMatrix", projection);
		
		for (auto i = 0; i < objectPositions.size(); i++)
		{
			glm::mat4 modelMatrix(1.0f);
			modelMatrix = glm::translate(modelMatrix, objectPositions[i]);
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 0.5f, 0.5f));
			shaderGeometryPass.setMat4("modelMatrix", modelMatrix);
			nanosuit.Draw(shaderGeometryPass);
		}
		shaderGeometryPass.unUse();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//2.Lighing Pass:calculate lighting by iterating over a screen filled quad pixel-by-pixel using the gbuffer's content.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		//设置点光源参数

		shaderLightingPass.use();
		for (auto i = 0; i < lightPositions.size(); i++)
		{
			
			string strParamName = string("lights[") + to_string(i) + string("].");
			string str = strParamName + POSITION_PARAM_NAME;
			shaderLightingPass.setVec3(str, glm::vec3(lightPositions[i].x, lightPositions[i].y+yOffset, lightPositions[i].z + zOffset ));
			str = strParamName + ATTENUATION_CONSTANT_PARAM_NAME;
			shaderLightingPass.setFloat(str, constant);

			str = strParamName + ATTENUATION_LINEAR_PARAM_NAME;
			shaderLightingPass.setFloat(str, linear);

			str = strParamName + ATTENUATION_QUADRATIC_PARAM_NAME;
			shaderLightingPass.setFloat(str, quadratic);

			str = strParamName + "color";
			shaderLightingPass.setVec3(str, lightColors[i]);

			str = strParamName + "radius";
			//计算光体积的半径
			const float maxBrightness = std::fmaxf(std::fmaxf(lightColors[i].r, lightColors[i].g), lightColors[i].b);
			float radius = (-linear + std::sqrt(linear * linear - 4 * quadratic * (constant - (256.0f / 5.0f) * maxBrightness))) / (2.0f * quadratic);
			shaderLightingPass.setFloat(str, radius);
		}

		shaderLightingPass.setBool("lightVolumeState", volumes);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPositionTextureId);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormalTextureId);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gAlbedoSpecTextureId);
		shaderLightingPass.setVec3("eyePos", camera.Position);
		RenderQuad();
		shaderLightingPass.unUse();
		//copy content of geometry's depth buffer to default framebuffer's depth buffer
		glBindFramebuffer(GL_READ_FRAMEBUFFER, gBufferFrameBufferId);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		//blit到默认帧缓冲区。请注意，这可能起作用，也可能不起作用，因为FBO和默认帧缓冲区的内部格式必须匹配。
		//内部格式由实现定义。这适用于我的所有系统，但如果你的系统不适用，你可能需要写信给
		//深度缓冲区在另一个着色器阶段（或者以某种方式查看以将默认帧缓冲区的内部格式与FBO的内部格式相匹配）。
		glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
		// the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the 		
		// depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).

		glBindVertexArray(cubeVAOId);
		shaderLightBox.use();
		shaderLightBox.setMat4("viewMatrix", viewMatrix);
		shaderLightBox.setMat4("projectionMatrix", projection);
		
		for (auto i = 0; i < lightPositions.size(); i++)
		{
			glm::mat4 modelMatrix(1.0f);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(lightPositions[i].x, lightPositions[i].y + yOffset, lightPositions[i].z+zOffset));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.125f, 0.125f, 0.125f));
			shaderLightBox.setMat4("modelMatrix", modelMatrix);
			shaderLightBox.setVec3("lightColor", lightColors[i]);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		
		shaderLightBox.unUse();
#pragma region create element
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::Begin("Hello Deferrred");
		//ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::DragFloat("light yoffset", &yOffset,0.1f,-5.f,5.f);      // Edit bools storing our window open/close state
		ImGui::DragFloat("light zoffset", &zOffset, 0.1f, -5.f, 5.f);														//ImGui::Checkbox("Another Window", &show_another_window);
		ImGui::Checkbox("light volumes", &volumes);
		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#pragma endregion
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glUseProgram(0);

		glfwSwapBuffers(window); // 交换缓存
	}
	// 释放资源
	/*glDeleteVertexArrays(1, &VAOId);
	glDeleteBuffers(1, &VBOId);*/
#pragma region Cleanup imgui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
#pragma endregion
	glfwTerminate();
	return 0;
}
//准备Quad Buffer，将FrameBuffer中的内容填充到quad中。
bool PrepareQuadBuffer()
{
	bool bRet = false;
	vector<vertex_attribute> vecAttrib;
	map<vertex_attribute, int> mapAttrib2Size;
	vecAttrib.emplace_back(vertex_attribute::position);
	vecAttrib.emplace_back(vertex_attribute::texcoord);
	mapAttrib2Size[vertex_attribute::position] = 3;
	mapAttrib2Size[vertex_attribute::texcoord] = 2;
	VAOBuffer quadVaoBuffer;
	quadVaoBuffer.BuildVAO(squareVertices, sizeof(squareVertices), squareIndexes, sizeof(squareIndexes), vecAttrib, mapAttrib2Size);
	// 创建缓存对象
	quadVAOId = quadVaoBuffer.GetVAO();
	quadVBOId = quadVaoBuffer.GetVBO();
	bRet = true;
	return bRet;
}
bool PrepareCubeBuffer()
{
	bool bRet = false;
	vector<vertex_attribute> vecAttrib;
	map<vertex_attribute, int> mapAttrib2Size;
	vecAttrib.emplace_back(vertex_attribute::position);
	vecAttrib.emplace_back(vertex_attribute::texcoord);
	mapAttrib2Size[vertex_attribute::position] = 3;
	mapAttrib2Size[vertex_attribute::texcoord] = 2;
	VAOBuffer cubeVaoBuffer;
	cubeVaoBuffer.BuildVAO(cubeVertices3, sizeof(cubeVertices3), nullptr, 0, vecAttrib, mapAttrib2Size);
	// 创建缓存对象
	cubeVAOId = cubeVaoBuffer.GetVAO();
	cubeVBOId = cubeVaoBuffer.GetVBO();
	bRet = true;
	return bRet;
}
void RenderQuad()
{
	glBindVertexArray(quadVAOId);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, squareIndexes);
	glBindVertexArray(0);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE); // 关闭窗口
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