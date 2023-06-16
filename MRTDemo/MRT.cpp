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
#include "learnopengl/Maroc.h"
#include "learnopengl/vaoBuffer.h"
#include "learnopengl/vertexset.h"
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
	// �����������
	
	VAOBuffer vaoBuffer;
	
	vector<vertex_attribute> vecVertexAttrib;
	vecVertexAttrib.push_back(vertex_attribute::position);
	vecVertexAttrib.push_back(vertex_attribute::texcoord);
	map< vertex_attribute, int> mapAttrib2Num;
	mapAttrib2Num[vertex_attribute::position] = 3;
	mapAttrib2Num[vertex_attribute::texcoord] = 2;
	vaoBuffer.BuildVAO(squareVertices, sizeof(squareVertices), 
		squareIndexes, sizeof(squareIndexes), 
		vecVertexAttrib, mapAttrib2Num);
	GLuint VAOId, VBOId, EBOId;
	VAOId = vaoBuffer.GetVAO();
	VBOId = vaoBuffer.GetVBO();
	EBOId = vaoBuffer.GetEBO();

	//����FBO
	GLuint mrtFrameBufferId;
	glGenFramebuffers(1, &mrtFrameBufferId);
	glBindFramebuffer(GL_FRAMEBUFFER, mrtFrameBufferId);
	const int nAttachment = 4;
	GLenum attatchments[nAttachment] = { 
		GL_COLOR_ATTACHMENT0, // ԭͼ
		GL_COLOR_ATTACHMENT1, // ��ɫ
		GL_COLOR_ATTACHMENT2, //��ɫ
		GL_COLOR_ATTACHMENT3 };// ��ɫ

	GLuint textureIds[nAttachment] = {};
	glGenTextures(nAttachment, textureIds);
	for (auto i = 0; i < nAttachment; i++)
	{
		glBindTexture(GL_TEXTURE_2D, textureIds[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, attatchments[i], GL_TEXTURE_2D, textureIds[i], 0);
	}
	glDrawBuffers(nAttachment, attatchments);
	if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER))
	{
		//todo
	}
	//glGenTextures()
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Section2 ׼����ɫ������
	int imgHeight = 0;
	int imgWidth = 0;
	GLuint texId = TextureFromFile("cat.png","../resources/textures",imgWidth, imgHeight);
	Shader shader("triangle.vertex", "triangle.frag");
	Shader mrtShader("MRT.vertex", "MRT.frag");
	shader.use();
	GL_INPUT_ERROR
	shader.setInt("tex", 0);
	shader.unUse();

	mrtShader.use();
	for (int i = 0; i < nAttachment; i++)
	{
		string strname = "tex" + std::to_string(i);
		mrtShader.setInt(strname, i);
	}
	mrtShader.unUse();
	// �����ӿڲ���
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	// ��ʼ��Ϸ��ѭ��
	// �����ɫ������ ����Ϊָ����ɫ
	glClearColor(0.18f, 0.04f, 0.14f, 1.0f);
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents(); // ����������� ���̵��¼�
		glBindFramebuffer(GL_FRAMEBUFFER, mrtFrameBufferId);
		// �����ӿڲ���
		//glViewport(0, 0, imgWidth, imgHeight);
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawBuffers(nAttachment, attatchments);
		// ������д�������ƴ���
		glBindVertexArray(VAOId);
		shader.use();
		//�����ȼ���ٽ��а󶨡�
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texId);
		
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, squareIndexes);
		shader.unUse();
		
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		//glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT);
		mrtShader.use();
		for (int i = 0; i < nAttachment; i++)
		{
			//�����ȼ���ٽ��а󶨡�
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, textureIds[i]);
			
		}
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, squareIndexes);
		mrtShader.unUse();
		glBindVertexArray(0);
		glUseProgram(0);

		glfwSwapBuffers(window); // ��������
	}
	// �ͷ���Դ
	glDeleteVertexArrays(1, &VAOId);
	glDeleteBuffers(1, &VBOId);
	glDeleteBuffers(1, &EBOId);
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