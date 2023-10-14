// ����GLEW�� ���徲̬����
#define GLEW_STATIC
#include <GLEW/glew.h>
// ����GLFW��
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

// ������ɫ�����ؿ�
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
// ���̻ص�����ԭ������
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

// ���������
const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;
bool CreateVertexSet(GLfloat** ptrBuffer, int& count);

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

	auto ptrModel = new Model("../Model/backpack/backpack.obj");

	//׼������
	GLuint diffuseMap = TextureFromFile("diffuse.jpg", "../Model/backpack");
	GLuint normalMap  = TextureFromFile("normal.png",  "../Model/backpack");
	//�������
	BoundingBox box = ptrModel->GetBoundingBox();
	
	glm::vec3 center = box.GetCenter();
	glm::vec3 position = center +  (box.GetLength()*3.f)*glm::vec3(0, 0, 1.0f);
	Camera camera(position);
	// Section2 ׼����ɫ������
	Shader shader("NormalMapWithComplexModel.vertex", "NormalMapWithComplexModel.frag");

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	// ��ʼ��Ϸ��ѭ��
	GLfloat angle = 0.0f;	
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents(); // ����������� ���̵��¼�

		// �����ɫ������ ����Ϊָ����ɫ
		glClearColor(0.18f, 0.04f, 0.14f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		// ������д�������ƴ���
		//glBindVertexArray(VAOId);
		shader.use();
		glm::mat4 model(1.f);
		
		angle += 0.015f;
		if (angle > 360.f)
		{
			angle-=360;
		}
		glm::quat q = glm::quat(glm::vec3(angle, angle, angle));
		model = glm::mat4_cast(q)*model;
		BoundingBox currentBox = box.Transformed(model);
		glm::vec3 currentCenter = box.GetCenter();
		

		//camera.Position = currentCenter+ (box.GetLength())*glm::vec3(0, 0, 1.0f);
		camera.Front = glm::normalize(currentCenter - camera.Position);
	
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(45.0f, (GLfloat)WINDOW_WIDTH / WINDOW_HEIGHT, 0.01f, 1000.0f);
		shader.setMat4("model", model);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		shader.setVec3("eyePos", camera.Position);
		
		
		glm::vec3 ambient(0.3f, 0.3f, 0.3f);
		glm::vec3 diffuse(0.8f, 0.8f, 0.8f);
		glm::vec3 spacular(1.0f, 1.0f, 1.0f);

		
		shader.setVec3("light.ambient", ambient);
		shader.setVec3("light.diffuse", diffuse);
		shader.setVec3("light.spacular", spacular);
		shader.setFloat("shiness", 256.0f);

		glm::vec3 lightPos = glm::vec3(1.2f, 1.0f, 2.0f);
		shader.setVec3("lightPos", lightPos);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normalMap);

		ptrModel->Draw(shader);


		//glBindVertexArray(0);
		glUseProgram(0);

		glfwSwapBuffers(window); // ��������
	}
	// �ͷ���Դ
	//glDeleteVertexArrays(1, &VAOId);
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
