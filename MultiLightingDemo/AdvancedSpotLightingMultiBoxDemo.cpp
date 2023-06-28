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
	
	// positions all containers
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

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
	GLuint diffuseTextureId = TextureFromFile("container2.png", "../resources/textures");
	GLuint spacularTextureId = TextureFromFile("container_specular.png", "../resources/textures");

	//�������
	BoundingBox box;
	int nVal = sizeof(cubeVertices) / sizeof(GLfloat);
	for(int i = 0; i< nVal; i+=8)
	{ 
		glm::vec3 pnt(cubeVertices[i], cubeVertices[i + 1], cubeVertices[i + 2]);
		box.Merge(pnt);
	}
	int nCube = sizeof(cubePositions) / sizeof(glm::vec3);
	BoundingBox totalBox;
	vector<glm::mat4> vecModelMatrix;
	vecModelMatrix.reserve(nCube);
	for (int i = 0; i < nCube; i++)
	{
		glm::mat4 modelMatirx(1.f);
		modelMatirx = glm::translate(modelMatirx, cubePositions[i]);
		BoundingBox tempBox = box.Transformed(modelMatirx);
		totalBox.Merge(tempBox);
		vecModelMatrix.emplace_back(modelMatirx);
	}
	glm::vec3 center = totalBox.GetCenter();
	float radius = totalBox.GetLength()*0.6f;
	glm::vec3 position = center + radius *glm::vec3(0, 0, 1.0f);
	Camera camera(position);
	
	// Section2 ׼����ɫ������
	Shader shader("advance_spot_caster_multi_box.vertex", "advance_spot_caster_multi_box.frag");
	// ��ʼ���ƹ�
	//LightManager lightManager;
	glm::vec3 ambient(0.3f, 0.3f, 0.3f);
	glm::vec3 diffuse(0.8f, 0.8f, 0.8f);
	glm::vec3 spacular(1.0f, 1.0f, 1.0f);
	//lightManager.CreateLight(LightType::SPOT_LIGHT, ambient, diffuse, spacular);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	// ��ʼ��Ϸ��ѭ��
	
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
		//glm::mat4 model(1.f);
		
		
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(45.0f, (GLfloat)WINDOW_WIDTH / WINDOW_HEIGHT, 0.01f, 1000.0f);
		
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		shader.setVec3("eyePos", camera.Position);

		shader.setVec3("material.ambient", ambient);
		shader.setInt("material.diffuse", 0);
		shader.setInt("material.spacular", 1);
		shader.setFloat("material.shiness", 256.0f);
		//������Ʒŵ������λ����Ҫ��ʾ�ƹ�ģ�ͣ�������ᱻ�ƹ���ȫ�ڵ��������������е��κ����塣
		//������Ʒŵ������λ���Ҳ���Ҫ��ʾ�ƹ�ģ�ͣ��������ȷ����Ⱦ����������
		float tv = glfwGetTime();
		
		glm::vec3 lightPos = camera.Position;
		lightPos.x = radius * glm::sin(tv);
		lightPos.z = radius * fabs(glm::cos(tv));
		glm::vec3 lightDirection = glm::normalize(center - lightPos);
		float cutoff = glm::cos(glm::radians(3.5f));
		float outerCutoff = glm::cos(glm::radians(5.0f));
		Spotlight spotlight(ambient, diffuse, spacular, lightPos, lightDirection,cutoff, outerCutoff);
		string strName = "light.";
		spotlight.SetLightUniformParam(shader, strName);
		//glm::vec3 lightPos = glm::vec3(1.2f, 1.0f, 2.0f);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseTextureId);

		glActiveTexture(GL_TEXTURE0+1);
		glBindTexture(GL_TEXTURE_2D, spacularTextureId);
		for (int i = 0; i < nCube; i++)
		{
			shader.setMat4("model", vecModelMatrix[i]);
			glDrawArrays(GL_TRIANGLES, 0, nVetex);
		}
		glBindVertexArray(0);
		glUseProgram(0);

		glfwSwapBuffers(window); // ��������
	}
	// �ͷ���Դ
	glDeleteVertexArrays(1, &VAOId);
	GLuint vboId = vaoBuffer.GetVBO();
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