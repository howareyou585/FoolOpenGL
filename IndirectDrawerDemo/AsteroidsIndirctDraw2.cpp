// ����GLEW�� ���徲̬����
#define GLEW_STATIC
#include <GLEW/glew.h>
// ����GLFW��
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>

// ������ɫ�����ؿ�
#include "learnopengl/shader.h"
#include "learnopengl/filesystem.h"
#include "learnopengl/model.h"
#include "learnopengl/camera.h"
#include "learnopengl/indirectCommandBuffer.h"
#include "learnopengl/VAOBuffer.h"

// ���̻ص�����ԭ������
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
// ���������
const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;

int main(int argc, char** argv)
{
	
	if (!glfwInit())	// ��ʼ��glfw��
	{
		std::cout << "Error::GLFW could not initialize GLFW!" << std::endl;
		return -1;
	}

	// ����OpenGL 3.3 core profile
	std::cout << "Start OpenGL core profile version 3.3" << std::endl;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// ��������
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
		"Demo of triangle", NULL, NULL);
	if (!window)
	{
		std::cout << "Error::GLFW could not create winddow!" << std::endl;
		glfwTerminate();
		return -1;
	}
	// �����Ĵ��ڵ�contextָ��Ϊ��ǰcontext
	glfwMakeContextCurrent(window);

	// ע�ᴰ�ڼ����¼��ص�����
	glfwSetKeyCallback(window, key_callback);

	// ��ʼ��GLEW ��ȡOpenGL����
	glewExperimental = GL_TRUE; // ��glew��ȡ������չ����
	GLenum status = glewInit();
	if (status != GLEW_OK)
	{
		std::cout << "Error::GLEW glew version:" << glewGetString(GLEW_VERSION) 
			<< " error string:" << glewGetErrorString(status) << std::endl;
		glfwTerminate();
		return -1;
	}

	// �����ӿڲ���
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	// Section2 ׼����ɫ������
	Shader rockShader("instanceIndirectDraw2.vertex", "instanceIndirectDraw2.frag");
	Shader plantShader("instance.vertex", "instance.frag");
	int error = glGetError();
	if (error != 0)
	{
		std::cout << "error:" << error << std::endl;
	}
	Model rock(FileSystem::getPath("Model/rock/rock.obj"));
	Model planet(FileSystem::getPath("Model/planet/planet.obj"));
	unsigned int amount = 10000;
	vector<glm::mat4> vecRockModelMatrix;
	vecRockModelMatrix.reserve(amount);
	vecRockModelMatrix.resize(amount);
	
	
	vector<Model*> vecRock;
	vecRock.push_back(&rock);
	VAOBuffer vaoBuffer(vecRock);
	vaoBuffer.Bind();

	GLuint drawer_id_buffer;
	glGenBuffers(1, &drawer_id_buffer);
	GL_INPUT_ERROR
	glBindBuffer(GL_ARRAY_BUFFER, drawer_id_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLuint) * amount, nullptr, GL_STATIC_DRAW);
	GL_INPUT_ERROR
	GLuint* ptrDrawId = (GLuint*)glMapBufferRange(
		GL_ARRAY_BUFFER,
		0,
		sizeof(GLuint) * amount,
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	for (int i = 0; i < amount; i++)
	{
		ptrDrawId[i] = i;
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);
	GL_INPUT_ERROR
	glEnableVertexAttribArray(3);
	glVertexAttribIPointer(3, 1, GL_UNSIGNED_INT, 0, 0);
	glVertexAttribDivisor(3, 1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	GL_INPUT_ERROR

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
	BoundingBox totalBoundingBox;
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
	// ��ʼ��Ϸ��ѭ��
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents(); // ����������� ���̵��¼�
		
		// �����ɫ������ ����Ϊָ����ɫ
		glClearColor(0.18f, 0.04f, 0.14f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		// ������д�������ƴ���
		plantShader.use();
		plantShader.setMat4("model", planetModelMatrix);
		planet.Draw(plantShader);
		plantShader.unUse();

		rockShader.use();
		float timeValue = glfwGetTime();
		rockShader.setFloat("time", timeValue);
		int error = glGetError();
		if (error != 0)
		{
			std::cout << "error:" << error << endl;
		}
		vaoBuffer.Bind();
		
		auto& vecTexture = vaoBuffer.GetTextures();
		for (int i = 0; i < vecTexture.size(); i++)
		{
			auto& texture = vecTexture[i];
			glBindTexture(GL_TEXTURE_2D, texture.id);
			string strName = texture.type + std::to_string(i + 1);
			rockShader.setInt(strName, i);
			glActiveTexture(GL_TEXTURE0);
			break;
		}
		
		//vaoBuffer.BindEBO();
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirect_draw_buffer);
		glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, NULL, amount, 0);
		GL_INPUT_ERROR
		
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
		//vaoBuffer.UnBindEBO();
		vaoBuffer.UnBind();
		rockShader.unUse();
		glfwSwapBuffers(window); // ��������
	}
	// �ͷ���Դ
	glfwTerminate();
	return 0;
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE); // �رմ���
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