// ����GLEW�� ���徲̬����
#define GLEW_STATIC
#include <GLEW/glew.h>
// ����GLFW��
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtc/matrix_transform.hpp>
// ������ɫ�����ؿ�
#include "learnopengl/shader.h"
#include "learnopengl/camera.h"
#include "learnopengl/model.h"
#include "learnopengl/vertexset.h"
#include "learnopengl/vaobuffer.h"
#include "learnopengl/skybox.h"
// ���̻ص�����ԭ������
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

// ���������
const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

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

	vector<vertex_attribute> vecAttrib;
	map<vertex_attribute, int> mapAttrib2Size;
	vecAttrib.emplace_back(vertex_attribute::position);
	vecAttrib.emplace_back(vertex_attribute::texcoord);
	mapAttrib2Size[vertex_attribute::position] = 3;
	mapAttrib2Size[vertex_attribute::texcoord] = 2;
	VAOBuffer cubeVaoBuffer;
	cubeVaoBuffer.BuildVAO(cubeVertices3, sizeof(cubeVertices3), nullptr, 0, vecAttrib, mapAttrib2Size);
	// �����������
	GLuint cubeVAOId = cubeVaoBuffer.GetVAO();
	GLuint cubeVBOId = cubeVaoBuffer.GetVBO();

	std::vector<string> vecFilePath;
	vecFilePath.push_back("../resources/skyboxes/sky/sky_lf.jpg");
	vecFilePath.push_back("../resources/skyboxes/sky/sky_rt.jpg");
	vecFilePath.push_back("../resources/skyboxes/sky/sky_up.jpg");
	vecFilePath.push_back("../resources/skyboxes/sky/sky_dn.jpg");
	vecFilePath.push_back("../resources/skyboxes/sky/sky_bk.jpg");
	vecFilePath.push_back("../resources/skyboxes/sky/sky_ft.jpg");
	SkyBox skyBox(vecFilePath);
	
	// Section2 ׼����ɫ������
	Shader skyboxShader("skybox.vertex","skybox.frag");
	Shader shader("scene.vertex", "scene.frag");

	glEnable(GL_DEPTH_TEST);//��Ȳ���
	glEnable(GL_CULL_FACE);//�޳���
	// ��ʼ��Ϸ��ѭ��
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents(); // ����������� ���̵��¼�

		// �����ɫ������ ����Ϊָ����ɫ
		glClearColor(0.18f, 0.04f, 0.14f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(0);
		glUseProgram(0);

		glfwSwapBuffers(window); // ��������
	}
	// �ͷ���Դ
	glDeleteVertexArrays(1, &cubeVAOId);
	glDeleteBuffers(1, &cubeVBOId);
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