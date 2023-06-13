// ����GLEW�� ���徲̬����
#define GLEW_STATIC
#include <GLEW/glew.h>
// ����GLFW��
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

// ������ɫ�����ؿ�
#include <glm/glm.hpp>
#include "learnopengl/shader.h"
#include "learnopengl/model.h"
#include "learnopengl/filesystem.h"


// ���̻ص�����ԭ������
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

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
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	// Section1 ׼����������
	// ָ�������������� ����λ��
	GLfloat vertices[] = {
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, //0
		1.0f, -1.0f, 0.0f,	1.0f, 0.0f, //1
		1.0f, 1.0f,  0.0f,	1.0f,1.0f,  //2
		/*-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, //0
		1.0f, 1.0f, 0.0f,   1.0f,1.0f,*/ //2
		-1.0f, 1.0f, 0.0f,  0.0f,1.0f  //3
	};
	GLuint indexes[] = {0,1,2,0,2,3};
	// �����������
	GLuint VAOId, VBOId, EBOId;
	// Step1: ��������VAO����
	glGenVertexArrays(1, &VAOId);
	glBindVertexArray(VAOId);
	// Step2: ��������VBO����
	glGenBuffers(1, &VBOId);
	glBindBuffer(GL_ARRAY_BUFFER, VBOId);
	// Step3: ����ռ� ��������
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// Step4: ָ��������ʽ  �����ö�������
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glGenBuffers(1, &EBOId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOId);
	// Step3: ����ռ� ��������
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexes), indexes, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	GLuint mrtFrameBufferId;
	glGenFramebuffers(1, &mrtFrameBufferId);
	glBindFramebuffer(GL_FRAMEBUFFER, mrtFrameBufferId);
	
	//glGenTextures()
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Section2 ׼����ɫ������
	GLuint texId = TextureFromFile("/resources/textures/cat.png","E:/github/FoolOpenGL");
	Shader shader("triangle.vertex", "triangle.frag");
	shader.use();
	shader.setInt("txt", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texId);
	shader.unUse();
	// ��ʼ��Ϸ��ѭ��
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents(); // ����������� ���̵��¼�

		// �����ɫ������ ����Ϊָ����ɫ
		glClearColor(0.18f, 0.04f, 0.14f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// ������д�������ƴ���
		glBindVertexArray(VAOId);
		shader.use();
		glBindTexture(GL_TEXTURE_2D, texId);
		//glDrawArrays(GL_TRIANGLES, 0, 6);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, indexes);
		glBindVertexArray(0);
		glUseProgram(0);

		glfwSwapBuffers(window); // ��������
	}
	// �ͷ���Դ
	glDeleteVertexArrays(1, &VAOId);
	glDeleteBuffers(1, &VBOId);
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