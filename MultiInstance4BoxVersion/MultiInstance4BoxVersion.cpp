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
#include "learnopengl/vertexset.h"
#include "learnopengl/vaobuffer.h"
#include <thread>

// ���̻ص�����ԭ������
void processInput(GLFWwindow* window, Camera& camera);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);// ���������
void UpdateInstanceMatrix(vector<glm::mat4>& vecInsMatrix);
void UpdateInstanceColor(vector<glm::vec3>& vecInsColor);
const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;
const int AMOUNT = 100;
float deltaTime = 0.0f; // ��ǰ֡����һ֡��ʱ���
float lastFrame = 0.0f; // ��һ֡��ʱ��
float lastX = WINDOW_WIDTH / 2.0f;
float lastY = WINDOW_HEIGHT / 2.0f;
bool  bFirstMove = true;
bool  bSwitchColorVBO = false;
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
	GLuint vaoId = vaoBuffer.GetVAO();
	//׼������
	GLuint diffuseTextureId = TextureFromFile("container_diffuse.png", "../resources/textures");
	GLuint spacularTexureId = TextureFromFile("container_specular.png", "../resources/textures");
	//�������
	BoundingBox box;
	int nVal = sizeof(cubeVertices) / sizeof(GLfloat);
	for (int i = 0; i < nVal; i += 8)
	{
		glm::vec3 pnt(cubeVertices[i], cubeVertices[i + 1], cubeVertices[i + 2]);
		box.Merge(pnt);
	}
	
	vector<glm::mat4>vecCubeModelMatrix;
	vector<glm::mat4>vecCubeModelMatrix2;
	vecCubeModelMatrix.reserve(AMOUNT);
	vecCubeModelMatrix2.reserve(AMOUNT);
	vector<glm::vec3>vecCubeModelColor(AMOUNT, glm::vec3(1.0f, 0.0f, 1.0f));
	vector<glm::vec3>vecCubeModelColor2(AMOUNT,glm::vec3(1.0f,1.0f,0.0f));
	

	UpdateInstanceMatrix(vecCubeModelMatrix);
	std::this_thread::sleep_for(std::chrono::seconds(1));
	UpdateInstanceMatrix(vecCubeModelMatrix2);
	
	BoundingBox totalCubeBoundingBox;
	for (auto & modelMatrix : vecCubeModelMatrix)
	{
		BoundingBox tempBox = box.Transformed(modelMatrix);
		totalCubeBoundingBox.Merge(tempBox);
	}
	camera.InitCamera(totalCubeBoundingBox, 0.8f);
	auto err = glGetError();
	if (0 != err)
	{
		std::cout << "error" << std::endl;
	}
	
	unsigned int insMatrixVBOId;
	glGenBuffers(1, &insMatrixVBOId);
	glBindBuffer(GL_ARRAY_BUFFER, insMatrixVBOId);
	glBufferData(GL_ARRAY_BUFFER, vecCubeModelMatrix.size() * sizeof(glm::mat4), vecCubeModelMatrix.data(),GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	unsigned int insMatrixVBOId2;
	glGenBuffers(1, &insMatrixVBOId2);
	glBindBuffer(GL_ARRAY_BUFFER, insMatrixVBOId2);
	glBufferData(GL_ARRAY_BUFFER, vecCubeModelMatrix2.size() * sizeof(glm::mat4), vecCubeModelMatrix2.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	unsigned int insColorVBOId;
	glGenBuffers(1, &insColorVBOId);
	glBindBuffer(GL_ARRAY_BUFFER, insColorVBOId);
	glBufferData(GL_ARRAY_BUFFER, vecCubeModelColor.size() * sizeof(glm::vec3), vecCubeModelColor.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	unsigned int insColorVBOId2;
	glGenBuffers(1, &insColorVBOId2);
	glBindBuffer(GL_ARRAY_BUFFER, insColorVBOId2);
	glBufferData(GL_ARRAY_BUFFER, vecCubeModelColor2.size() * sizeof(glm::vec3), vecCubeModelColor2.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	err = glGetError();
	if (0 != err)
	{
		std::cout << "error" << std::endl;
	}
	// Section2 ׼����ɫ������
	Shader shader("instance.vertex", "instance.frag");
	shader.use();
	
	glm::mat4 viewMatrix = camera.GetViewMatrix();
	shader.setMat4("view", viewMatrix);
	glm::mat4 projection = camera.GetProjectionMatrix((GLfloat)(WINDOW_WIDTH) / (GLfloat)(WINDOW_HEIGHT));
	shader.setMat4("projection", projection);
	shader.unUse();
	
	glEnable(GL_DEPTH_TEST);
	// �����ɫ������ ����Ϊָ����ɫ
	glClearColor(0.18f, 0.04f, 0.14f, 1.0f);
	
	// ��ʼ��Ϸ��ѭ��
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame; // ��һ֡��ʱ��
		processInput(window, camera);
		glfwPollEvents(); // ����������� ���̵��¼�
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if (bSwitchColorVBO)
		{
			glBindVertexArray(vaoId);
			glBindBuffer(GL_ARRAY_BUFFER, insColorVBOId2);
			vecCubeModelColor2.clear();
			UpdateInstanceColor(vecCubeModelColor2);
			glBufferSubData(GL_ARRAY_BUFFER, 0, vecCubeModelColor2.size() * sizeof(glm::vec3), vecCubeModelColor2.data());
			glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
			glEnableVertexAttribArray(7);
			glVertexAttribDivisor(7, 1);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindBuffer(GL_ARRAY_BUFFER, insMatrixVBOId2);
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
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindVertexArray(0);
			auto err = glGetError();
			if (0 != err)
			{
				std::cout << "error" << std::endl;
			}
			bSwitchColorVBO = !bSwitchColorVBO;
		}
		else
		{
			glBindVertexArray(vaoId);
			glBindBuffer(GL_ARRAY_BUFFER, insColorVBOId);
			vecCubeModelColor.clear();
			UpdateInstanceColor(vecCubeModelColor);
			glBufferSubData(GL_ARRAY_BUFFER, 0, vecCubeModelColor.size() * sizeof(glm::vec3), vecCubeModelColor.data());
			glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
			glEnableVertexAttribArray(7);
			glVertexAttribDivisor(7, 1);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindBuffer(GL_ARRAY_BUFFER, insMatrixVBOId);
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
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindVertexArray(0);
			auto err = glGetError();
			if (0 != err)
			{
				std::cout << "error" << std::endl;
			}
			bSwitchColorVBO = !bSwitchColorVBO;
			
		}
		

		// ������д�������ƴ���
		glBindVertexArray(vaoId);
		
		shader.use();
		glDrawArraysInstanced(GL_TRIANGLES, 0, 36, AMOUNT);
		shader.unUse();
		glBindVertexArray(0);
		glfwSwapBuffers(window); // ��������
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	// �ͷ���Դ
	glDeleteVertexArrays(1, &vaoId);
	//glDeleteBuffers(1,  &vaoBuffer.GetVBO());
	glfwTerminate();
	return 0;
}

void UpdateInstanceMatrix(vector<glm::mat4>& vecInsMatrix)
{
	vector<glm::mat4>().swap(vecInsMatrix);
	srand(static_cast<unsigned int>(glfwGetTime())); // initialize random seed
	float radius = 5.0;
	float offset = 0.25f;
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
void UpdateInstanceColor(vector<glm::vec3>& vecColor)
{
	for (unsigned int i = 0; i < AMOUNT; i++)
	{
		// Also calculate random color
		GLfloat rColor = ((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0
		GLfloat gColor = ((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0
		GLfloat bColor = ((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0
		vecColor.push_back(glm::vec3(rColor, gColor, bColor));
	}
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE); // �رմ���
	}
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		bSwitchColorVBO = !bSwitchColorVBO;
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