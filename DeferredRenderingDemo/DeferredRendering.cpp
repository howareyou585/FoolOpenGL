// ����GLEW�� ���徲̬����
#define GLEW_STATIC
#include <GLEW/glew.h>
// ����GLFW��
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
// ������ɫ�����ؿ�
#include "learnopengl/boundingbox.h"
#include "learnopengl/shader.h"
#include "learnopengl/model.h"
#include "learnopengl/filesystem.h"
#include "learnopengl/camera.h"
#include "learnopengl/fboBuffer.h"

// ���̻ص�����ԭ������
void processInput(GLFWwindow* window, Camera& camera);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);// ���������
void UpdateInstanceMatrix(vector<glm::mat4>& vecInsMatrix);
const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;
float deltaTime = 0.0f; // ��ǰ֡����һ֡��ʱ���
float lastFrame = 0.0f; // ��һ֡��ʱ��
float lastX = WINDOW_WIDTH / 2.0f;
float lastY = WINDOW_HEIGHT / 2.0f;
bool  bFirstMove = true;
Camera camera;

GLuint quadVAOId = 0, quadVBOId = 0;
bool PrepareQuadBuffer();
void RenderQuad();

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
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	//��������Ҫ����GLFW����Ӧ�����ع�꣬����׽(Capture)����
	//��׽����ʾ���ǣ������������ĳ�����
	//����ע������ʾ�����ڲ����������Windows��ӵ�н���ĳ��������ͨ��������ɫ���Ǹ���
	//��ʧȥ����ĳ�����������ǻ�ɫ�ģ������Ӧ��ͣ���ڴ����У����ǳ���ʧȥ��������˳�����
	//���ǿ�����һ���򵥵����õ�������ɣ�
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//�ڵ����������֮������������ôȥ�ƶ���꣬��궼������ʾ�ˣ�
	// ��Ҳ�����뿪���ڡ�����FPS�����ϵͳ��˵�ǳ�������
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
	Model nanosuit(FileSystem::getPath("Model/nanosuit/nanosuit.obj"));
	std::vector<glm::vec3> objectPositions;
	objectPositions.push_back(glm::vec3(-10.0, -10.0, -10.0));
	objectPositions.push_back(glm::vec3(0.0, -10.0, -10.0));
	objectPositions.push_back(glm::vec3(10.0, -10.0, -10.0));
	objectPositions.push_back(glm::vec3(-10.0, -10.0, 0.0));
	objectPositions.push_back(glm::vec3(0.0, -10.0, 0.0));
	objectPositions.push_back(glm::vec3(10.0, -10.0, 0.0));
	objectPositions.push_back(glm::vec3(-10.0, -10.0, 10.0));
	objectPositions.push_back(glm::vec3(0.0, -10.0, 10.0));
	objectPositions.push_back(glm::vec3(10.0, -10.0, 10.0));
	BoundingBox sceneBoundingBox;
	BoundingBox& box = nanosuit.GetBoundingBox();
	glm::mat4 modelMatrix(1.0f);
	for (auto& item : objectPositions)
	{
		glm::mat4 tempModelMatrix = glm::translate(modelMatrix, item);
		BoundingBox tempBoundingBox = box.Transformed(tempModelMatrix);
		sceneBoundingBox.Merge(tempBoundingBox);
	}
	camera.InitCamera(sceneBoundingBox, 1.8f);
	auto length = sceneBoundingBox.GetLength() * 1.8f;
	// Section2 ׼����ɫ������
	Shader shaderGeometryPass("deferred_shading.vertex", "deferred_shading.frag");
	glm::vec3 ambient(0.3f, 0.3f, 0.3f);
	glm::vec3 diffuse(0.8f, 0.8f, 0.8f);
	glm::vec3 spacular(1.0f, 1.0f, 1.0f);
	shaderGeometryPass.use();
	shaderGeometryPass.setVec3("light.ambient", ambient);
	shaderGeometryPass.setVec3("light.diffuse", diffuse);
	shaderGeometryPass.setVec3("light.spacular", spacular);
	shaderGeometryPass.setVec3("material.ambient", ambient);
	shaderGeometryPass.setVec3("spacular", spacular);
	shaderGeometryPass.setFloat("shiness", 256.0f);
	shaderGeometryPass.unUse();

	map<GLenum, AttachmentType> mapColorAttachment;
	mapColorAttachment[GL_COLOR_ATTACHMENT0] = AttachmentType::Texture;
	mapColorAttachment[GL_COLOR_ATTACHMENT1] = AttachmentType::Texture;
	mapColorAttachment[GL_COLOR_ATTACHMENT2] = AttachmentType::Texture;
	FBOBuffer fboBuffer(mapColorAttachment, true, false);

	glEnable(GL_DEPTH_TEST);
	// ��ʼ��Ϸ��ѭ��
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
		//glBindVertexArray(VAOId);
		shaderGeometryPass.use();
		/*glm::mat4 modelMatrix(1.0f);
		shader.setMat4("model", modelMatrix);*/
		glm::mat4 viewMatrix = camera.GetViewMatrix();
		shaderGeometryPass.setMat4("view", viewMatrix);
		glm::mat4 projection = camera.GetProjectionMatrix((GLfloat)(WINDOW_WIDTH) / (GLfloat)(WINDOW_HEIGHT));
		shaderGeometryPass.setMat4("projection", projection);
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		shaderGeometryPass.setVec3("eyePos", camera.Position);
		float angle = glfwGetTime();
		
		shaderGeometryPass.setVec3("light.position",glm::vec3(length *glm::sin(angle),
			camera.Position.y, length*glm::cos(angle)));
		for (auto i = 0; i < objectPositions.size(); i++)
		{
			glm::mat4 modelMatrix(1.0f);
			modelMatrix = glm::translate(modelMatrix, objectPositions[i]);
			shaderGeometryPass.setMat4("model", modelMatrix);
			nanosuit.Draw(shaderGeometryPass);
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glUseProgram(0);

		glfwSwapBuffers(window); // ��������
	}
	// �ͷ���Դ
	/*glDeleteVertexArrays(1, &VAOId);
	glDeleteBuffers(1, &VBOId);*/
	glfwTerminate();
	return 0;
}
//׼��Quad Buffer����FrameBuffer�е�������䵽quad�С�
bool PrepareQuadBuffer()
{
	bool bRet = false;
	bRet = true;
	return bRet;
}

void RenderQuad()
{
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE); // �رմ���
	}
}
void processInput(GLFWwindow* ptrWindow, Camera& camera)
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
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);
	if (bFirstMove)
	{
		lastX = xpos;
		lastY = ypos;
		bFirstMove = false;
	}

	float offsetX = xpos - lastX;
	float offsetY = lastY - ypos; // y ��������Խ��Խ��
	//std::cout << "lastY="<<lastY <<",ypos =" <<ypos<< ",offsetY =" <<offsetY << endl;
	//std::cout << "offsetX=" << offsetX << ",offsetY=" << offsetY << std::endl;
	lastX = xpos;
	lastY = ypos;
	camera.ProcessMouseMovement(offsetX, offsetY);
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	//std::cout << "xoffset=" << xoffset << ",yoffset=" << yoffset << std::endl;
	camera.ProcessMouseScroll(yoffset);
}