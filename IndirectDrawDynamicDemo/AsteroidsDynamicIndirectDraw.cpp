// 引入GLEW库 定义静态链接
#define GLEW_STATIC
#include <GLEW/glew.h>
// 引入GLFW库
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

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


struct vp_matrix_buffer
{
	glm::mat4 view_matrix;
	glm::mat4 projection_matrix;
	glm::mat4 vp_matrix;
};

struct material_property
{
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec3 specular;
	float shiness;
};
// 定义程序常量
const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;
const unsigned int amount = 16384;
unsigned int realRenderModelCount = amount;
const unsigned int material_count = 100;
const unsigned int addend = 512;

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
	Shader rockShader("DynamicRockIndirect.vertex", "DynamicRockIndirect.frag");
	Shader plantShader("PlanetInstance.vertex", "PlanetInstance.frag");
	int error = glGetError();
	if (error != 0)
	{
		std::cout << "error:" << error << std::endl;
	}
	Model rock(FileSystem::getPath("Model/rock/rock.obj"));
	Model planet(FileSystem::getPath("Model/planet/planet.obj"));
	
	vector<Model*> vecRock;
	vecRock.push_back(&rock);
	VAOBuffer vaoBuffer(vecRock);
	vaoBuffer.Bind();
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
		ptrIndirectCommands[i].baseInstance = i%100;//为什么？
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
	BoundingBox totalBoundingBox;
	totalBoundingBox.Merge(tempBoundingBox);
	float length = totalBoundingBox.GetLength() * 1.8;

	glm::vec3 targetPos = totalBoundingBox.GetCenter();
	glm::vec3 eysPos = targetPos + glm::vec3(0, 0, 1) * length;
	Camera camera(eysPos);


	glm::mat4 view = camera.GetViewMatrix();
	float rad = glm::radians(camera.Zoom);
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (GLfloat)WINDOW_WIDTH / WINDOW_HEIGHT, 0.01f, 1000.0f);
	/*rockShader.use();
	rockShader.setMat4("view", view);
	rockShader.setMat4("projection", projection);
	rockShader.unUse();*/
	//初始化rock的矩阵：model_matrix;
	GLuint rock_model_matrix_shared_buffer;
	glGenBuffers(1, &rock_model_matrix_shared_buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, rock_model_matrix_shared_buffer);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, 
		sizeof(glm::mat4)* amount, 
		nullptr, GL_MAP_WRITE_BIT);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	//初始化rock的矩阵：view_matrix, projection_matrix.
	GLuint vp_matrix_uniforms_buffer;
	glGenBuffers(1, &vp_matrix_uniforms_buffer);
	glBindBuffer(GL_UNIFORM_BUFFER, vp_matrix_uniforms_buffer);
	glBufferStorage(GL_UNIFORM_BUFFER, sizeof(vp_matrix_buffer), NULL, GL_MAP_WRITE_BIT);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, vp_matrix_uniforms_buffer);
	
	vp_matrix_buffer* ptrVPMatrix =
		(vp_matrix_buffer*)glMapBufferRange(
			GL_UNIFORM_BUFFER, 0,
			sizeof(vp_matrix_buffer),
			GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	ptrVPMatrix->view_matrix = view;
	ptrVPMatrix->projection_matrix = projection;
	ptrVPMatrix->vp_matrix = projection * view;
	glUnmapBuffer(GL_UNIFORM_BUFFER);

	//初始化rock 实例的材质
	GLuint rock_matrial_buffer;
	glGenBuffers(1, &rock_matrial_buffer);
	glBindBuffer(GL_UNIFORM_BUFFER, rock_matrial_buffer);
	glBufferStorage(GL_UNIFORM_BUFFER, sizeof(material_property)* material_count, NULL, GL_MAP_WRITE_BIT);
	material_property* ptrMaterialProperty = 
		(material_property*)glMapBufferRange(
								GL_UNIFORM_BUFFER,  0, 
								sizeof(material_property) * material_count,
								GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	for (int i = 0; i < material_count; i++)
	{
		float f = ((float)(i)) / ((float)material_count);
		ptrMaterialProperty[i].ambient =  (glm::vec4(sinf(f * 3.7f), sinf(f * 5.7f + 3.0f), sinf(f * 4.3f + 2.0f), 1.0f) 
			+ glm::vec4(1.0f, 1.0f, 2.0f, 0.0f)) * 0.1f;
		ptrMaterialProperty[i].diffuse =  (glm::vec4(sinf(f * 9.9f + 6.0f), sinf(f * 3.1f + 2.5f), sinf(f * 7.2f + 9.0f), 1.0f) 
			+ glm::vec4(1.0f, 2.0f, 2.0f, 0.0f)) * 0.4f;
		ptrMaterialProperty[i].specular = (glm::vec3(sinf(f * 1.6f + 4.0f), sinf(f * 0.8f + 2.7f), sinf(f * 5.2f + 8.0f)) 
			+ glm::vec3(19.0f, 19.0f, 19.0f)) * 0.6f;
		ptrMaterialProperty[i].shiness = 200.0f + sinf(f) * 50.0f;
	}

	glBindBuffer(GL_UNIFORM_BUFFER, 0);

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
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 这里填写场景绘制代码
		plantShader.use();
		planetModelMatrix = glm::rotate(planetModelMatrix, (float)(glfwGetTime()) / 1000.f, glm::vec3(0, 1, 0));
		plantShader.setMat4("model", planetModelMatrix);
		planet.Draw(plantShader);
		plantShader.unUse();
		if (realRenderModelCount)
		{
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, rock_model_matrix_shared_buffer);
			glm::mat4* ptrRockModelMatrix = (glm::mat4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER,
				0, sizeof(glm::mat4) * realRenderModelCount,
				GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, rock_model_matrix_shared_buffer);
			float f = (float)glfwGetTime() * 0.1f;
			for (int i = 0; i < realRenderModelCount; i++)
			{

				glm::mat4 rockModelMatrix = glm::mat4(1.0f);
				rockModelMatrix = glm::translate(rockModelMatrix, glm::vec3(sinf(f * 7.3f) * 70.0f, sinf(f * 3.7f + 2.0f) * 70.0f, sinf(f * 2.9f + 8.0f) * 70.0f));
				/*glm::quat q = glm::quat(glm::radians(glm::vec3(f * 330.0f, f * 490.0f, f * 250.0f)));
				rockModelMatrix = glm::mat4_cast(q) * rockModelMatrix;*/

				ptrRockModelMatrix[i] = rockModelMatrix;
				f += 3.1f;
			}
			glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
			glBindBufferBase(GL_UNIFORM_BUFFER, 2, rock_matrial_buffer);

			rockShader.use();
			vaoBuffer.Bind();
			//vaoBuffer.BindEBO();
			glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirect_draw_buffer);
			glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, NULL, realRenderModelCount, 0);

			glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
			//vaoBuffer.UnBindEBO();
			vaoBuffer.UnBind();
			rockShader.unUse();
		}
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
	else if (key == GLFW_KEY_A && action == GLFW_PRESS)
	{
		realRenderModelCount += addend;
		if (realRenderModelCount > amount)
		{
			realRenderModelCount = amount;
		}
	}
	else if (key == GLFW_KEY_Z && action == GLFW_PRESS)
	{
		realRenderModelCount -= addend;
		if (realRenderModelCount >amount)
		{
			realRenderModelCount = 0;
		}
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