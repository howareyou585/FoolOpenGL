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
#include "learnopengl/light.h"
#include "learnopengl/lightManager.h"
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

	
	// �����������
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
	GLuint VAOId = vaoBuffer.GetVAO();
	//׼������
	GLuint textureId = TextureFromFile("container2.png", "../resources/textures");
	//�������
	BoundingBox box;
	int nVal = sizeof(cubeVertices) / sizeof(GLfloat);
	for(int i = 0; i< nVal; i+=8)
	{ 
		glm::vec3 pnt(cubeVertices[i], cubeVertices[i + 1], cubeVertices[i + 2]);
		box.Merge(pnt);
	}
	glm::vec3 center = box.GetCenter();
	glm::vec3 position = center +  (box.GetLength()*3.f)*glm::vec3(0, 0, 1.0f);
	Camera camera(position);
	// Section2 ׼����ɫ������
	Shader shader("multi_lighting.vertex", "multi_lighting.frag");
	Shader lightCubeShader("light_cube.vertex", "light_cube.frag");
	// 
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	// ��ʼ��Ϸ��ѭ��
	GLfloat angle = 0.0f;
	int nVetex = nVal / 8; //���������
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents(); // ����������� ���̵��¼�

		// �����ɫ������ ����Ϊָ����ɫ
		glClearColor(0.18f, 0.04f, 0.14f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		// ������д�������ƴ���
		glBindVertexArray(VAOId);
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
		shader.setVec3("material.ambient", ambient);
		shader.setInt("material.diffuse", 0);
		shader.setVec3("material.spacular", spacular);
		shader.setFloat("material.shiness", 256.0f);

		glm::vec3 lightPos = camera.Position;
		shader.setVec3("light.position", lightPos);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureId);
		glDrawArrays(GL_TRIANGLES, 0, nVetex);

		lightCubeShader.use();
		
		glm::mat4 cubeModel(1.f);
		
		cubeModel = glm::translate(cubeModel, lightPos);
		cubeModel = glm::scale(cubeModel, glm::vec3(0.15f, 0.15f, 0.15f));

		lightCubeShader.setMat4("model", cubeModel);
		lightCubeShader.setMat4("view", view);
		lightCubeShader.setMat4("projection", projection);
		glDrawArrays(GL_TRIANGLES, 0, nVetex);
		glBindVertexArray(0);
		glUseProgram(0);

		glfwSwapBuffers(window); // ��������
	}
	// �ͷ���Դ
	glDeleteVertexArrays(1, &VAOId);
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