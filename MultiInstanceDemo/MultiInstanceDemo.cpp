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

// ���̻ص�����ԭ������
void processInput(GLFWwindow* window, Camera& camera);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);// ���������
void UpdateInstanceMatrix(vector<glm::mat4>& vecInsMatrix);
const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;
const int AMOUNT = 100;
float deltaTime = 0.0f; // ��ǰ֡����һ֡��ʱ���
float lastFrame = 0.0f; // ��һ֡��ʱ��
float lastX = WINDOW_WIDTH / 2.0f;
float lastY = WINDOW_HEIGHT / 2.0f;
bool  bFirstMove = true;
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
	// �����������
	//GLuint VAOId =0, VBOId = 0;
	GLuint insMatrixVBOId;
	glGenBuffers(1, &insMatrixVBOId);
	glBindBuffer(GL_ARRAY_BUFFER, insMatrixVBOId);
	glBufferData(GL_ARRAY_BUFFER, AMOUNT * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
	for (auto& mesh : nanosuit.meshes)
	{
		glBindVertexArray(mesh.VAO);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)0);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(sizeof(float) * 4));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(sizeof(float) * 8));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(sizeof(float) * 12));
		glEnableVertexAttribArray(6);

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
		glBindVertexArray(mesh.VAO);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	BoundingBox& box = nanosuit.GetBoundingBox();
	glm::vec3 position = box.GetCenter();
	float length = box.GetLength()*200;
	position = position + length * glm::vec3(0.f, 0.f, 1.f);
	camera.SetPosition(position);
	
	// Section2 ׼����ɫ������
	Shader shader("instance.vertex", "instance.frag");
	glm::vec3 ambient(0.3f, 0.3f, 0.3f);
	glm::vec3 diffuse(0.8f, 0.8f, 0.8f);
	glm::vec3 spacular(1.0f, 1.0f, 1.0f);
	shader.use();
	shader.setVec3("light.ambient", ambient);
	shader.setVec3("light.diffuse", diffuse);
	shader.setVec3("light.spacular", spacular);
	shader.setVec3("material.ambient", ambient);
	shader.setVec3("spacular", spacular);
	shader.setFloat("shiness", 256.0f);
	shader.unUse();
	vector<glm::mat4> vecInsMatrix;
	vecInsMatrix.reserve(AMOUNT);
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

		UpdateInstanceMatrix(vecInsMatrix);
		glBindBuffer(GL_ARRAY_BUFFER, insMatrixVBOId);
		glBufferSubData(GL_ARRAY_BUFFER, 0, vecInsMatrix.size() * sizeof(glm::mat4), vecInsMatrix.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// ������д�������ƴ���
		//glBindVertexArray(VAOId);
		shader.use();
		/*glm::mat4 modelMatrix(1.0f);
		shader.setMat4("model", modelMatrix);*/
		glm::mat4 viewMatrix = camera.GetViewMatrix();
		shader.setMat4("view", viewMatrix);
		glm::mat4 projection = camera.GetProjectionMatrix((GLfloat)(WINDOW_WIDTH) / (GLfloat)(WINDOW_HEIGHT));
		shader.setMat4("projection", projection);
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		shader.setVec3("eyePos", camera.Position);
		float angle = glfwGetTime();
		
		shader.setVec3("light.position",glm::vec3(length *glm::sin(angle),
			camera.Position.y, length*glm::cos(angle)));
		nanosuit.MultiInstanceDraw(shader, AMOUNT);
		//nanosuit.Draw(shader);
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

void UpdateInstanceMatrix(vector<glm::mat4>& vecInsMatrix)
{
	vector<glm::mat4>().swap(vecInsMatrix);
	srand(static_cast<unsigned int>(glfwGetTime())); // initialize random seed
	float radius = 500.0;
	float offset = 25.0f;
	for (unsigned int i = 0; i < AMOUNT; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		// 1. translation: displace along circle with 'radius' in range [-offset, offset]
		float angle = (float)i / (float)AMOUNT * 360.0f;
		float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float x = sin(angle) * radius + displacement;
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float z = cos(angle) * radius + displacement;
		model = glm::translate(model, glm::vec3(x, y, z));

		// 2. scale: Scale between 0.05 and 0.25f
		float scale = static_cast<float>((rand() % 20) / 100.0 + 0.05);
		model = glm::scale(model, glm::vec3(scale));

		// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
		float rotAngle = static_cast<float>((rand() % 360));
		model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));
		vecInsMatrix.emplace_back(model);
	}
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