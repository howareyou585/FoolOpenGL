// 引入GLEW库 定义静态链接
#define GLEW_STATIC
#include <GLEW/glew.h>
// 引入GLFW库
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>

// 包含着色器加载库
#include "learnopengl/shader.h"
#include "learnopengl/filesystem.h"
#include "learnopengl/model.h"
#include "learnopengl/camera.h"
#include "learnopengl/indirectCommandBuffer.h"
#include "learnopengl/VAOBuffer.h"

// 键盘回调函数原型声明
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
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
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	// Section2 准备着色器程序
	Shader rockShader("instanceIndirectDraw.vertex", "instanceIndirectDraw.frag");
	Shader plantShader("instance.vertex", "instance.frag");
	int error = glGetError();
	if (error != 0)
	{
		std::cout << "error:" << error << std::endl;
	}
	Model rock(FileSystem::getPath("Model/rock/rock.obj"));
	Model planet(FileSystem::getPath("Model/planet/planet.obj"));
	unsigned int amount = 1000;
	vector<glm::mat4> vecRockModelMatrix;
	vecRockModelMatrix.reserve(amount);
	vecRockModelMatrix.resize(amount);
	BoundingBox totalBoundingBox;
	
	srand(static_cast<unsigned int>(glfwGetTime())); // initialize random seed
	float radius = 50.0;
	float offset = 2.5f;
	for (unsigned int i = 0; i < amount; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		// 1. translation: displace along circle with 'radius' in range [-offset, offset]
		float angle = (float)i / (float)amount * 360.0f;
		float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float x = sin(angle) * radius + displacement;
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float z = cos(angle) * radius + displacement;
		model = glm::translate(model, glm::vec3(x, y, z));

		// 2. scale: Scale between 0.05 and 0.25f
		float scale = static_cast<float>((rand() % 20) / 100.0 + 0.05);
		model = glm::scale(model, glm::vec3(scale));

		// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
		float rotAngle = static_cast<float>((rand() % 360));
		model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

		// 4. now add to list of matrices
		vecRockModelMatrix[i] = model;
		// 5.merge boundingbox
		auto& boundingBox = rock.GetBoundingBox();
		BoundingBox tempBoundingBox = boundingBox.Transformed(model);
		totalBoundingBox.Merge(tempBoundingBox);
	}
	vector<Model*> vecRock;
	vecRock.push_back(&rock);
	VAOBuffer vaoBuffer(vecRock);

	vaoBuffer.Bind();
	GLuint instanceMatrixBuffer;
	int mat4Size = sizeof(glm::mat4);
	int vec4Size = sizeof(glm::vec4);
	glGenBuffers(1, &instanceMatrixBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, instanceMatrixBuffer);
	glBufferData(GL_ARRAY_BUFFER, mat4Size * amount, vecRockModelMatrix.data(), GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, mat4Size, (void*)0);
	
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, mat4Size, (void*)(vec4Size));

	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, mat4Size, (void*)(vec4Size * 2));
	
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, mat4Size, (void*)(vec4Size * 3));
	
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLuint indirect_draw_buffer;
	glGenBuffers(1, &indirect_draw_buffer);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirect_draw_buffer);
	glBufferStorage(GL_DRAW_INDIRECT_BUFFER, amount * sizeof(DrawElementsIndirectCommand), nullptr, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
	error = glGetError();
	if (error != 0)
	{
		std::cout << "error:" << error << std::endl;
	}
	DrawElementsIndirectCommand* ptrIndirectCommands = (DrawElementsIndirectCommand*)glMapBufferRange(
		GL_DRAW_INDIRECT_BUFFER, 
		0, 
		sizeof(DrawElementsIndirectCommand) * amount, 
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	error = glGetError();
	if (error != 0)
	{
		std::cout << "error:" << error << std::endl;
	}
	for (int i = 0; i < amount; i++)
	{
		ptrIndirectCommands[i].baseInstance = i;
		ptrIndirectCommands[i].firstIndex = 0;
		ptrIndirectCommands[i].primCount = 1;
		ptrIndirectCommands[i].baseVertex = 0;
		ptrIndirectCommands[i].count = vaoBuffer.m_vecIndex.size();
	}
	glUnmapBuffer(GL_DRAW_INDIRECT_BUFFER);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
	vaoBuffer.UnBind();
	
	auto& planetBoundingBox = planet.GetBoundingBox();
	glm::mat4 planetModelMatrix = glm::mat4(1.0f);
	planetModelMatrix = glm::translate(planetModelMatrix, glm::vec3(0.0f, -3.0f, 0.0f));
	planetModelMatrix = glm::scale(planetModelMatrix, glm::vec3(4.0f, 4.0f, 4.0f));
	BoundingBox tempBoundingBox = planetBoundingBox;
	tempBoundingBox = tempBoundingBox.Transformed(planetModelMatrix);
	totalBoundingBox.Merge(tempBoundingBox);
	float length = totalBoundingBox.GetLength()*1.8;

	glm::vec3 targetPos = totalBoundingBox.GetCenter();
	glm::vec3 eysPos = targetPos + glm::vec3(0, 0, 1) *length;
	Camera camera(eysPos);
	
	
	glm::mat4 view = camera.GetViewMatrix();
	float rad = glm::radians(camera.Zoom);
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (GLfloat)WINDOW_WIDTH / WINDOW_HEIGHT, 0.01f, 1000.0f);
	rockShader.use();
	rockShader.setMat4("view", view);
	rockShader.setMat4("projection", projection);
	rockShader.unUse();

	plantShader.use();
	plantShader.setMat4("view", view);
	plantShader.setMat4("projection", projection);
	plantShader.unUse();
	// 开始游戏主循环
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents(); // 处理例如鼠标 键盘等事件

		// 清除颜色缓冲区 重置为指定颜色
		glClearColor(0.18f, 0.04f, 0.14f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		// 这里填写场景绘制代码
		/*plantShader.use();
		plantShader.setMat4("model", planetModelMatrix);
		planet.Draw(plantShader);
		plantShader.unUse();*/

		rockShader.use();
		/*for (auto i = 0; i < vecRockModelMatrix.size(); i++)
		{
			glm::mat4& model = vecRockModelMatrix[i];
			rockShader.setMat4("model", model);
			error = glGetError();
			if (error != 0)
			{
				std::cout << "error:" << error << std::endl;
			}
			rock.Draw(rockShader);
		}*/
		int error = glGetError();
		if (error != 0)
		{
			std::cout << "error:" << error << endl;
		}
		vaoBuffer.Bind();
		//vaoBuffer.BindEBO();
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirect_draw_buffer);
		glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, NULL, amount, 0);
		GL_INPUT_ERROR
		
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
		//vaoBuffer.UnBindEBO();
		vaoBuffer.UnBind();
		rockShader.unUse();
		glfwSwapBuffers(window); // 交换缓存
	}
	// 释放资源
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
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	/*if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);*/
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	//if (firstMouse)
	//{
	//	lastX = xpos;
	//	lastY = ypos;
	//	firstMouse = false;
	//}

	//float xoffset = xpos - lastX;
	//float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	//lastX = xpos;
	//lastY = ypos;

	//camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	//camera.ProcessMouseScroll(static_cast<float>(yoffset));
}