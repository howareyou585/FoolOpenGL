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
#include "learnopengl/vaoBuffer.h"
#include "learnopengl/vertexset.h"
#include "learnopengl/boundingbox.h"
#include "learnopengl/model.h"
#include "learnopengl/camera.h"
// ���̻ص�����ԭ������
//void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow *window, Camera & camera);
void mouseClick_callback(GLFWwindow* window, int button, int action, int mods);
glm::vec3 GetWorldPosFromViewport(int winX, int winY);
// ���������
const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;
float deltaTime = 0.0f; // ��ǰ֡����һ֡��ʱ���
float lastFrame = 0.0f; // ��һ֡��ʱ��
Camera camera;
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
	glfwSetMouseButtonCallback(window, mouseClick_callback);
	// �����Ĵ��ڵ�contextָ��Ϊ��ǰcontext
	glfwMakeContextCurrent(window);

	// ע�ᴰ�ڼ����¼��ص�����
	//glfwSetKeyCallback(window, key_callback);

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
	VAOBuffer vaoBuffer;
	vector<vertex_attribute> vecAttrib;
	map<vertex_attribute, int> mapAttrib2Size;
	vecAttrib.emplace_back(vertex_attribute::position);
	vecAttrib.emplace_back(vertex_attribute::texcoord);
	mapAttrib2Size[vertex_attribute::position] = 3;
	mapAttrib2Size[vertex_attribute::texcoord] = 2;
	vaoBuffer.BuildVAO(cubeVertices3, sizeof(cubeVertices3), nullptr, 
		0, vecAttrib, mapAttrib2Size);
	
	// �����������
	GLuint VAOId = vaoBuffer.GetVAO();
	GLuint VBOId = vaoBuffer.GetVBO();
	//���ز���
	GLuint texId  = TextureFromFile("container.jpg", "../resources/textures");
	GLuint texId2 = TextureFromFile("awesomeface.png", "../resources/textures");
	// world space positions of our cubes
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
	BoundingBox box;
	int nVal = sizeof(cubeVertices3) / sizeof(GLfloat);
	for (int i = 0; i < nVal; i += 5)
	{
		glm::vec3 pnt(cubeVertices3[i], cubeVertices3[i + 1], cubeVertices3[i + 2]);
		box.Merge(pnt);
	}
	BoundingBox totalBoundingBox;
	int nModelMatrix = sizeof(cubePositions) / sizeof(glm::vec3);
	vector<glm::mat4> vecModelMatrix;
	vecModelMatrix.reserve(nModelMatrix);
	for (int i = 0; i < nModelMatrix; i++)
	{
		glm::mat4 model(1.0f);
		model = glm::translate(model, cubePositions[i]);
		float angle = 20.0f * i;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		BoundingBox tmpBoundingBox = box.Transformed(model);
		totalBoundingBox.Merge(tmpBoundingBox);
		vecModelMatrix.emplace_back(model);
	}
	//float raduis = totalBoundingBox.GetLength()*0.8f;
	
	camera.InitCamera(totalBoundingBox,0.8f);
	float radius = totalBoundingBox.GetLength() * 0.8f;
	glm::mat4 projectionMatrix = camera.GetProjectionMatrix((float)WINDOW_WIDTH / (float)WINDOW_HEIGHT);
	// Section2 ׼����ɫ������
	Shader shader("cube.vertex", "cube.frag");
	shader.use();
	shader.setInt("s_texture", 0);
	shader.setInt("s_texture2", 1);
	shader.setMat4("projection", projectionMatrix);
	shader.unUse();
	int nVertex = sizeof(cubeVertices3) / (sizeof(GLuint) * 5);
	// ��ʼ��Ϸ��ѭ��
	glEnable(GL_DEPTH_TEST);
	glm::vec3 targetPos = totalBoundingBox.GetCenter();
	float distance = glm::length(targetPos - camera.Position);
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame; // ��һ֡��ʱ��
		processInput(window, camera);
		glfwPollEvents(); // ����������� ���̵��¼�

		// �����ɫ������ ����Ϊָ����ɫ
		glClearColor(0.18f, 0.04f, 0.14f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		// ������д�������ƴ���
		glBindVertexArray(VAOId);
		shader.use();
		//����ƶ�����ͷ���򲻱�
		targetPos = camera.Position + distance * camera.Front;
		glm::mat4 viewMatrix = camera.GetViewMatrix(targetPos);

		shader.setMat4("view", viewMatrix);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texId);
		glActiveTexture(GL_TEXTURE0+1);
		glBindTexture(GL_TEXTURE_2D, texId2);
		for (auto j = 0; j < nModelMatrix; j++)
		{
			shader.setMat4("model", vecModelMatrix[j]);
			glDrawArrays(GL_TRIANGLES, 0, nVertex);
		}
		shader.unUse();
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
void mouseClick_callback(GLFWwindow* window, int button, int action, int mods)
{
	if ((action == GLFW_PRESS) && (button == GLFW_MOUSE_BUTTON_RIGHT))
	{
		double winX{}, winY{};
		glfwGetCursorPos(window, &winX, &winY);
		glm::vec3 worldPosition = GetWorldPosFromViewport(winX, winY);
	}
}
void processInput(GLFWwindow *ptrWindow, Camera & camera)
{
	if (!ptrWindow)
	{
		return;
	}
	Camera_Movement direction;
	bool bMove = false;
	if (glfwGetKey(ptrWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(ptrWindow, true);

	}
	else if (glfwGetKey(ptrWindow, GLFW_KEY_W) == GLFW_PRESS)
	{
		direction = Camera_Movement::FORWARD;
		bMove = true;
		//camera.ProcessKeyboard()
	}
	else if (glfwGetKey(ptrWindow, GLFW_KEY_S) == GLFW_PRESS)
	{
		direction = Camera_Movement::BACKWARD;
		bMove = true;
		//camera.ProcessKeyboard()
	}
	else if (glfwGetKey(ptrWindow, GLFW_KEY_A) == GLFW_PRESS)
	{
		direction = Camera_Movement::LEFT;
		bMove = true;
		//camera.ProcessKeyboard()
	}
	else if (glfwGetKey(ptrWindow, GLFW_KEY_D) == GLFW_PRESS)
	{
		direction = Camera_Movement::RIGHT;
		bMove = true;
		//camera.ProcessKeyboard()
	}
	if (bMove)
	{
		camera.ProcessKeyboard(direction, deltaTime);
	}
}
glm::vec3 GetWorldPosFromViewport(int winX, int winY)
{
	float winZ{};
	glReadPixels((int)winX, (int)(WINDOW_HEIGHT - winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

	float x = 2.f * winX / WINDOW_WIDTH - 1.0f; // ����Ļ����---->[-1.0f,1.0f]������
	float y = 1.f - 2.f * winY / WINDOW_HEIGHT; // ����Ļ����---->[-1.0f,1.0f]������
	float z = winZ * 2.f - 1.f;//�������[0,1] ------->[-1.0f,1.0]
	glm::vec3 worldPosition(0.f, 0.f, 0.f);
	if (winZ < 1.0f)
	{
		glm::vec4 tempPosition;
		float w = (camera.NearPlane * camera.FarPlane) / (camera.NearPlane * winZ - camera.FarPlane * winZ + camera.FarPlane);
		tempPosition = glm::vec4(x, y, z, 1.0f);
		tempPosition *= w;
		glm::mat4 projectionMatrix = camera.GetProjectionMatrix(((float)WINDOW_WIDTH) / WINDOW_HEIGHT);
		glm::mat4 viewMatrix = camera.GetViewMatrix();
		tempPosition = glm::inverse(viewMatrix) * glm::inverse(projectionMatrix) * tempPosition;
		worldPosition.x = tempPosition.x;
		worldPosition.y = tempPosition.y;
		worldPosition.z = tempPosition.z;
		std::cout << "x:" << tempPosition.x << ",y:" << tempPosition.y << ",z:" << tempPosition.z << std::endl;
	}
	else
	{
		std::cout << "û������ѡ�С�" << endl;
	}

	return worldPosition;
}