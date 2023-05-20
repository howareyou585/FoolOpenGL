// ����GLEW�� ���徲̬����
//ʹ�ü��̲������
#define GLEW_STATIC
#include <GLEW/glew.h>
// ����GLFW��
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/postprocess.h>
// ������ɫ�����ؿ�
#include "shader.h"
#include "texture.h"
#include "Model.h"

// ���̻ص�����ԭ������
GLfloat yaw = 0.0f;
GLfloat pitch = 0.0f;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow*, double, double);
// ���������
const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;
glm::vec3 cameraPos(1.0f, 0.0f, 10.0f);
glm::vec3 cameraFront(0.f, 0.f, -1.0f);
glm::vec3 targetPos(0.f, 0.f, -1.0f);
glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);
glm::vec3 sideDir;
glm::vec3 forwardDir;
GLfloat length{};
bool bFirstMouseMove = true;
GLfloat lastX{};
GLfloat lastY{};
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
	glfwSetCursorPosCallback(window, mouse_callback);
	
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
//	GLfloat vertices[] = {
//-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,	// A
//0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,	// B
//0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,	    // C
//0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,	    // C
//-0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,	// D
//-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,	// A
//
//
//-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,	// E
//-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0, 1.0f,    // H
//0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,	// G
//0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,	// G
//0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,	// F
//-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,	// E
//
//-0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,	// D
//-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0, 1.0f,    // H
//-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,	// E
//-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,	// E
//-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,	// A
//-0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,	// D
//
//0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,	// F
//0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,	// G
//0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,	    // C
//0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,	    // C
//0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,	// B
//0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,	// F
//
//0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,	// G
//-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0, 1.0f,    // H
//-0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,	// D
//-0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,	// D
//0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,	// C
//0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,	// G
//
//-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,	// A
//-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,	// E
//0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,	// F
//0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,	// F
//0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,	// B
//-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,	// A
//	};

	/*glm::vec3 cubePostitions[] = {
		glm::vec3(0.0f, 0.0f, 1.2f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(1.2f, 1.2f, 0.0f),
		glm::vec3(-1.2f, 1.2f, 0.0f),
		glm::vec3(-1.2f, -1.5f, 0.0f),
		glm::vec3(1.2f, -1.5f, 0.0f),
		glm::vec3(0.0f, 0.0f, -1.2f),
	};*/
	//glm::vec3 cubePostitions[] = {
	//	glm::vec3(0.0f, 0.0f, 3.2f)/*,
	//	glm::vec3(0.0f, 0.0f, 0.0f),
	//	glm::vec3(1.2f, 2.2f, 0.0f),
	//	glm::vec3(-1.2f, 1.2f, 0.0f),
	//	glm::vec3(-1.2f, -2.5f, 0.0f),
	//	glm::vec3(1.2f, -1.5f, 0.0f),
	//	glm::vec3(0.0f, 0.0f, -3.2f),*/
	//};
	auto ptrModel = new Model();
	ptrModel->Load("../Model/male02/male02.obj", /*aiProcess_CalcTangentSpace
		|*/ aiProcess_Triangulate
		/*| aiProcess_JoinIdenticalVertices
		| aiProcess_SortByPType
		| aiProcess_SplitLargeMeshes
		| aiProcess_GenSmoothNormals*/
		| aiProcess_FlipUVs);
	// �����������
	//GLuint VAOId, VBOId;
	//// Step1: ��������VAO����
	//glGenVertexArrays(1, &VAOId);
	//glBindVertexArray(VAOId);
	//// Step2: ��������VBO����
	//glGenBuffers(1, &VBOId);
	//glBindBuffer(GL_ARRAY_BUFFER, VBOId);
	//// Step3: ����ռ� ��������
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//// Step4: ָ��������ʽ  �����ö�������
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)0);
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)(3 * sizeof(GL_FLOAT)));
	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)(6 * sizeof(GL_FLOAT)));
	//glEnableVertexAttribArray(2);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindVertexArray(0);

	//GLuint textureId = TextureHelper::load2DTexture("../resources/textures/cat.png");

	//// Section2 ׼����ɫ������
	Shader shader("cube.vertex", "cube.frag");
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);
	//shader.use();
	
	
	/*for (int i = 0; i < sizeof(cubePostitions) / sizeof(glm::vec3); i++)
	{*/

	/*}*/
	//shader.unUse();
	float angle = 0.0f;
	float boxLength = ptrModel->GetBoundingBoxLength();
	boxLength *= 0.8f;
	glm::vec3 center = ptrModel->m_center;
	targetPos = center;
	cameraPos = center;
	cameraPos.z = boxLength;
	length = boxLength;
	// ��ʼ��Ϸ��ѭ��
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents(); // ����������� ���̵��¼�

		// �����ɫ������ ����Ϊָ����ɫ
		glClearColor(0.18f, 0.04f, 0.14f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/*	cameraPos.x = glm::cos(glfwGetTime());
		cameraPos.z = glm::sin(glfwGetTime());*/
		//cameraPos = center - cameraFront * boxLength;
		
		//auto dir = glm::normalize(center - cameraPos);
		glm::mat4 model;
		model = glm::scale(model, glm::vec3(0.25, 0.25, 0.25));
		glm::mat4 view = glm::lookAt(cameraPos,  /*targetPos*//*cameraPos+cameraFront*/targetPos, cameraUp);
		glm::mat4 projection = glm::perspective(45.0f, (GLfloat)WINDOW_WIDTH / WINDOW_HEIGHT, 0.01f, 1000.0f);
		
		// ������д�������ƴ���
		//glBindVertexArray(VAOId);
		
		shader.use();
		/*glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureId);
		auto texLocationId = glGetUniformLocation(shader.programId, "tex");
		glUniform1i(texLocationId, 0);*/
		auto modelLocationId = glGetUniformLocation(shader.programId, "model");
		glUniformMatrix4fv(modelLocationId, 1, GL_FALSE, glm::value_ptr(model));
		auto viewLocationId = glGetUniformLocation(shader.programId, "view");
		glUniformMatrix4fv(viewLocationId, 1, GL_FALSE, glm::value_ptr(view));
		auto projectionLocationId = glGetUniformLocation(shader.programId, "projection");
		glUniformMatrix4fv(projectionLocationId, 1, GL_FALSE, glm::value_ptr(projection));
		/*vec3 direction;
		vec3 ambient;
		vec3 diffuse;
		vec3 spcular;*/
		auto lightDirection = glGetUniformLocation(shader.programId, "light.direction");
		glm::vec3 dir(30.f, 100.f, 10.f);
		glUniform3fv(lightDirection, 1, &dir[0]);
		
	
		ptrModel->Draw(shader.programId);
		//for (int i = 0; i < sizeof(cubePostitions) / sizeof(glm::vec3); i++)
		//{
		//	angle += 0.1f;
		//	model = glm::translate(model, cubePostitions[i]);
		//	view = glm::rotate(view, angle, glm::vec3(0.f, 1.0f, 0.f));
		//	//model = glm::rotate(model, angle, glm::vec3(0.f, 1.0f, 0.f));
			
		//	glDrawArrays(GL_TRIANGLES, 0, 36);
		//}
		
		

		//glBindVertexArray(0);
		glUseProgram(0);

		glfwSwapBuffers(window); // ��������
	}
	// �ͷ���Դ
	/*glDeleteVertexArrays(1, &VAOId);
	glDeleteBuffers(1, &VBOId);*/
	glfwTerminate();
	return 0;
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	float cameraSpeed = 0.5f; // adjust accordingly
	if (key == GLFW_KEY_ESCAPE /*&& action == GLFW_PRESS*/)
	{
		glfwSetWindowShouldClose(window, GL_TRUE); // �رմ���
	}
	else if (key == GLFW_KEY_W /*&& action == GLFW_PRESS*/)
	{
		//to up
		
		auto dir = glm::normalize(targetPos - cameraPos);
		cameraPos += cameraSpeed * dir;
		targetPos += cameraSpeed * dir;
	}
	else if (key == GLFW_KEY_S/* && action == GLFW_PRESS*/)
	{
		auto dir = glm::normalize(cameraPos - targetPos);
		cameraPos += cameraSpeed * dir;
		targetPos += cameraSpeed * dir;
	}
	else if (key == GLFW_KEY_D /*&& action == GLFW_PRESS*/)   //to right
	{
		//todo
		auto dir = glm::normalize(targetPos - cameraPos);
		
		glm::vec3 right = glm::normalize( glm::cross(dir, cameraUp));
		cameraPos += right * cameraSpeed;
		targetPos += cameraSpeed * right;

	}
	else if (key == GLFW_KEY_A /*&& action == GLFW_PRESS*/) //to left
	{
		//todo
		auto dir = glm::normalize(targetPos - cameraPos);
		glm::vec3 left =glm::normalize(glm::cross(cameraUp, dir));
		//right = glm::normalize(right);
		cameraPos += left * cameraSpeed;
		targetPos += cameraSpeed * left;
	}
	else if (key == GLFW_KEY_UP)
	{
		//���������ƶ��ķ���
		glm::vec3 right = glm::normalize(glm::cross(cameraUp, cameraFront));
		cameraPos += glm::normalize(glm::cross(cameraFront, right))*cameraSpeed;
	}

	else if (key == GLFW_KEY_DOWN)
	{
		//���������ƶ��ķ���
		glm::vec3 right = glm::normalize(glm::cross(cameraUp, cameraFront));
		cameraPos -= glm::normalize(glm::cross(cameraFront, right))*cameraSpeed;
	}
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (bFirstMouseMove)
	{
		lastX = xpos;
		lastY = ypos;
		bFirstMouseMove = false;
		return;
	}
	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = ypos - lastY;
	
	lastX = xpos;
	lastY = ypos;

	pitch += xoffset * 0.05;
	yaw += yoffset * 0.05;
	forwardDir.x = -sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	forwardDir.y = sin(glm::radians(pitch));
	forwardDir.z = -cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	forwardDir = glm::normalize(forwardDir);

	//glm::vec3 side;
	sideDir.x = cos(glm::radians(yaw));
	sideDir.y = 0;
	sideDir.z = -sin(glm::radians(yaw));
	sideDir = glm::normalize(sideDir);

	targetPos = cameraPos + glm::vec3(forwardDir.x * length, forwardDir.y * length, forwardDir.z * length);
	
}