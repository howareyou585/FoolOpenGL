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
	//glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	vector<vertex_attribute> vecVertexAttrib;
	vecVertexAttrib.push_back(vertex_attribute::position);
	vecVertexAttrib.push_back(vertex_attribute::texcoord);
	map< vertex_attribute, int> mapVertexAttrib2Num;
	mapVertexAttrib2Num[vertex_attribute::position] = 3;
	mapVertexAttrib2Num[vertex_attribute::texcoord] = 2;
	//����plane�������
	VAOBuffer quadVaoBuffer;
	quadVaoBuffer.BuildVAO(squareVertices, sizeof(squareVertices),
		squareIndexes, sizeof(squareIndexes), vecVertexAttrib, mapVertexAttrib2Num);
	// ����cube�������
	vecVertexAttrib.clear();
	vecVertexAttrib.push_back(vertex_attribute::position);
	vecVertexAttrib.push_back(vertex_attribute::normal);
	vecVertexAttrib.push_back(vertex_attribute::texcoord);
	mapVertexAttrib2Num.clear();
	mapVertexAttrib2Num[vertex_attribute::position] = 3;
	mapVertexAttrib2Num[vertex_attribute::normal] = 3;
	mapVertexAttrib2Num[vertex_attribute::texcoord] = 2;
	VAOBuffer cubeVaoBuffer;
	cubeVaoBuffer.BuildVAO(cubeVertices2, sizeof(cubeVertices2),
		nullptr, 0, vecVertexAttrib, mapVertexAttrib2Num);

	VAOBuffer planeVaoBuffer;
	planeVaoBuffer.BuildVAO(planeVertices, sizeof(planeVertices),
		 nullptr, 0,
		 vecVertexAttrib, mapVertexAttrib2Num);
	
	GLuint cubeVAOId = cubeVaoBuffer.GetVAO();
	GLuint planeVAOId = planeVaoBuffer.GetVAO();
	GLuint quadVAOId = quadVaoBuffer.GetVAO();

	
	//׼������
	GLuint woodTexId = TextureFromFile("wood.png", "../resources/textures");
	//configure depth map FBO
	const unsigned int SHADOW_WIDTH = 1024;
	const unsigned int SHADOW_HEIGTH = 1024;
	GLuint depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER,depthMapFBO);
	GLuint depthMapTextureID;
	glGenTextures(1, &depthMapTextureID);
	glBindTexture(GL_TEXTURE_2D, depthMapTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGTH,
		0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTextureID, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//�������
	BoundingBox box;
	int nVal = sizeof(cubeVertices2) / sizeof(GLfloat);
	for(int i = 0; i< nVal; i+=8)
	{ 
		glm::vec3 pnt(cubeVertices2[i], cubeVertices2[i + 1], cubeVertices2[i + 2]);
		box.Merge(pnt);
	}
	vector<glm::mat4> vecModelMatrix;
	//instance1
	glm::mat4 model(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
	model = glm::scale(model, glm::vec3(0.5f));
	vecModelMatrix.emplace_back(model);
	//instance2 
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
	model = glm::scale(model, glm::vec3(0.5f));
	vecModelMatrix.emplace_back(model);
	//instance3
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
	model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	model = glm::scale(model, glm::vec3(0.25));
	vecModelMatrix.emplace_back(model);
	BoundingBox totalBox;
	for (int i = 0; i < vecModelMatrix.size(); i++)
	{
		totalBox.Merge(box.Transformed(vecModelMatrix[i]));
	}
	// Section2 ׼����ɫ������
	Shader simpleDepthShader("shadow_mapping_depth.vertex", "shadow_mapping_depth.frag");
	Shader quadShader("quad.vertex", "quad.frag");
	glm::vec3 center = totalBox.GetCenter();
	float boxLength = totalBox.GetLength();
	glm::vec3 position = center + (boxLength *2.f)*glm::vec3(0, 0, 1.0f);
	Camera camera(position);
	glm::vec3 lightPos = center + (boxLength)*glm::vec3(0, 0, 1.0f);
	lightPos.y += boxLength * 0.5f;
	lightPos.x -= boxLength * 0.5f;
	//glm::vec3 lightPos = position;
	float nearPlane = 0.1f;
	float farPlane = boxLength *4.f;
	float val = boxLength;
	glm::mat4 lightProjection = glm::ortho(-val, val, -val, val, nearPlane, farPlane);
	//glm::mat4 cameraProjection = glm::perspective(45.0f, (GLfloat)WINDOW_WIDTH / WINDOW_HEIGHT, 0.01f, 1000.0f);
	glm::mat4 lightView = glm::lookAt(lightPos, 
		glm::vec3(0.f, 0.f, 0.f), 
		glm::vec3(0.f, 1.0f, 0.0f));
	
	simpleDepthShader.use();
	
	simpleDepthShader.setMat4("view", lightView);
	simpleDepthShader.setMat4("projection", lightProjection);
	/*simpleDepthShader.setMat4("view", camera.GetViewMatrix());
	simpleDepthShader.setMat4("projection", cameraProjection);*/
	simpleDepthShader.setInt("s_tex", 0);
	simpleDepthShader.unUse();
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	// ��ʼ��Ϸ��ѭ��
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents(); // ����������� ���̵��¼�

		// �����ɫ������ ����Ϊָ����ɫ
		glClearColor(0.18f, 0.04f, 0.14f, 1.0f);
		//�û���������framebuffer
		//glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGTH);
		// ������д�������ƴ���
		planeVaoBuffer.Bind();
		simpleDepthShader.use();
		model = glm::mat4(1.f);
		simpleDepthShader.setMat4("model", model);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, woodTexId);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		planeVaoBuffer.UnBind();

		cubeVaoBuffer.Bind();
		for (int i = 0; i < vecModelMatrix.size(); i++)
		{
			simpleDepthShader.setMat4("model", vecModelMatrix[i]);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		
		simpleDepthShader.unUse();

		//ϵͳĬ�ϵ�framebuffer
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
		//quadVaoBuffer.Bind();
		//
		//quadShader.use();
		//quadShader.setInt("depthMap", 0);
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, depthMapTextureID);
		////glDrawArrays(GL_TRIANGLES, 0, 6);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, squareIndexes);
		//quadShader.unUse();
		//quadVaoBuffer.UnBind();

		glfwSwapBuffers(window); // ��������
	}
	// �ͷ���Դ
	glDeleteVertexArrays(1, &cubeVAOId);
	GLuint cubeVboId = cubeVaoBuffer.GetVBO();
	glDeleteBuffers(1, &cubeVboId);

	glDeleteVertexArrays(1, &quadVAOId);
	GLuint quadVBOId = quadVaoBuffer.GetVBO();
	glDeleteBuffers(1, &quadVBOId);
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