// ����GLEW�� ���徲̬����
#define GLEW_STATIC
#include <GLEW/glew.h>
// ����GLFW��
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

// ������ɫ�����ؿ�
#include "learnopengl/shader.h"
#include "learnopengl/vaobuffer.h"
#include "learnopengl/model.h"

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
		//position				//color                  //texture_coord
		1.0f,3.f,0.f,           1.f,0.f,0.f,			 1.f,2.f,
		1.0f,-3.f, 0.f,         0.f,1.f,0.f,		     1.f,-1.f,
		-1.0f, -3.f, 0.f,		0.f,0.f,1.f,			 0.f,-1.f,
		-1.0f, 3.f, 0.f,		0.f,0.f,0.f,			 0.f,2.f
	};
	GLuint indices[] = { 0,1,3,1,2,3 };
	// �����������
	vector<vertex_attribute> vecAttrib;
	map<vertex_attribute, int> mapAttrib2Size;
	vecAttrib.emplace_back(vertex_attribute::position);
	vecAttrib.emplace_back(vertex_attribute::color);
	vecAttrib.emplace_back(vertex_attribute::texcoord);
	mapAttrib2Size[vertex_attribute::position] = 3;
	mapAttrib2Size[vertex_attribute::color] = 3;
	mapAttrib2Size[vertex_attribute::texcoord] = 2;
	VAOBuffer vaoBuffer;
	vaoBuffer.BuildVAO(vertices, sizeof(vertices), indices, sizeof(indices), vecAttrib, mapAttrib2Size);
	GLuint vaoId = vaoBuffer.GetVAO();
	GLuint vboId = vaoBuffer.GetVBO();
	GLuint texSnow1 = TextureFromFile("snow1.png", "../resources/textures");
	GLuint texSnow2 = TextureFromFile("snow2.png", "../resources/textures");
	// Section2 ׼����ɫ������
	Shader shader("Snowing.vertex", "Snowing.frag");
	shader.use();
	shader.setInt("s_tex1", 0);
	shader.setInt("s_tex2", 1);
	shader.unUse();
	int cnt = 0;
	int flag = 1;
	int N = 2000;
	// ��ʼ��Ϸ��ѭ��
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents(); // ����������� ���̵��¼�

		// �����ɫ������ ����Ϊָ����ɫ
		glClearColor(0.18f, 0.04f, 0.14f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// ������д�������ƴ���
		vaoBuffer.Bind();
		vaoBuffer.BindEBO();
		//glBindVertexArray(vaoId);
		shader.use();
		
		glActiveTexture( GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texSnow1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D,texSnow2);

		cnt += 1;
		if (cnt > 2 * N)
		{
			cnt = 0;
		}
		if (cnt == 0 || cnt == N)
		{
			flag = -flag;
		}
		float time = 0.f;
		if (flag == 1)
		{
			time = cnt * 1.0f / N;
			
		}
		else
		{
			time = 2.f - cnt * 1.0f / N;
		}
		shader.setFloat("time", time);
		glm::mat4 transform(1.f);
		transform = glm::translate(transform, glm::vec3(0.f, 1.0 - cnt * 1.0 / N, 0.0f));
		shader.setMat4("transform", transform);
		//glDrawElements(GL_TRIANGLES,6 ,GL_UNSIGNALED,indices);
		glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(sizeof(indices)), GL_UNSIGNED_INT, 0);
		vaoBuffer.UnBindEBO();
		vaoBuffer.UnBind();
		glUseProgram(0);

		glfwSwapBuffers(window); // ��������
	}
	// �ͷ���Դ
	glDeleteVertexArrays(1, &vaoId);
	glDeleteBuffers(1, &vboId);
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