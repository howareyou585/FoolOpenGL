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
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// ���������
const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;
float deltaTime = 0.0f; // ��ǰ֡����һ֡��ʱ���
float lastFrame = 0.0f; // ��һ֡��ʱ��
float lastX = WINDOW_WIDTH / 2.0f;
float lastY = WINDOW_HEIGHT / 2.0f;
bool  bFirstMove = true;
bool  hdrFlg = false;
float exposureValue = 1.0f;
bool hdrKeyPressed = false;
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

	// ע�ᴰ�ڼ����¼��ص�����
	//glfwSetKeyCallback(window, key_callback);
	// ע�ᴰ������¼��ص�����
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	//��������Ҫ����GLFW����Ӧ�����ع�꣬����׽(Capture)����
	//��׽����ʾ���ǣ������������ĳ�����
	//����ע������ʾ�����ڲ����������Windows��ӵ�н���ĳ��������ͨ��������ɫ���Ǹ���
	//��ʧȥ����ĳ�����������ǻ�ɫ�ģ������Ӧ��ͣ���ڴ����У����ǳ���ʧȥ��������˳�����
	//���ǿ�����һ���򵥵����õ�������ɣ�
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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
	VAOBuffer vaoBuffer;
	
	vector<vertex_attribute> vecAttrib;
	map<vertex_attribute, int> mapAttrib2Size;
	vecAttrib.emplace_back(vertex_attribute::position);
	vecAttrib.emplace_back(vertex_attribute::normal);
	vecAttrib.emplace_back(vertex_attribute::texcoord);
	mapAttrib2Size[vertex_attribute::position] = 3;
	mapAttrib2Size[vertex_attribute::normal] = 3;
	mapAttrib2Size[vertex_attribute::texcoord] = 2;
	vaoBuffer.BuildVAO(cubeVertices2, sizeof(cubeVertices2), nullptr, 
		0, vecAttrib, mapAttrib2Size);
	
	// �����������
	GLuint VAOId = vaoBuffer.GetVAO();
	GLuint VBOId = vaoBuffer.GetVBO();

	VAOBuffer quadVAOBuffer;
	
	vecAttrib.clear();
	mapAttrib2Size.clear();
	vecAttrib.emplace_back(vertex_attribute::position);
	vecAttrib.emplace_back(vertex_attribute::texcoord);
	mapAttrib2Size[vertex_attribute::position] = 3;
	mapAttrib2Size[vertex_attribute::texcoord] = 2;
	quadVAOBuffer.BuildVAO(squareVertices, sizeof(squareVertices), squareIndexes,
		sizeof(squareIndexes), vecAttrib, mapAttrib2Size);
	// �����������
	GLuint quadVAOId = quadVAOBuffer.GetVAO();
	GLuint quadVBOId = quadVAOBuffer.GetVBO();

	//���ز���
	GLuint woodTexId  = TextureFromFile("wood.png", "../resources/textures");
	// Light sources
   // - Positions
	vector<glm::vec3> lightPositions;
	lightPositions.emplace_back(glm::vec3(0.0f, 0.0f, 49.5f)); // �׹�λ��
	lightPositions.emplace_back(glm::vec3(-1.4f, -1.9f, 9.0f));// ���λ��
	lightPositions.emplace_back(glm::vec3(0.0f, -1.8f, 4.0f)); // ����λ��
	lightPositions.emplace_back(glm::vec3(0.8f, -1.7f, 6.0f)); // �̹�λ��
	// - Colors
	vector<glm::vec3> lightColors;
	lightColors.emplace_back(glm::vec3(200.f, 200.f, 200.f));// �׹�
	lightColors.emplace_back(glm::vec3(0.1f, 0.0f, 0.0f));	 // ���
	lightColors.emplace_back(glm::vec3(0.0f, 0.0f, 0.2f));	 // ����
	lightColors.emplace_back(glm::vec3(0.0f, 0.1f, 0.0f));	 // �̹�
	//lightColors.emplace_back(glm::vec3(100.1f, 100.1f, 100.1f));	 // ���
	//lightColors.emplace_back(glm::vec3(200.2f, 200.2f, 200.2f));	 // ����
	//lightColors.emplace_back(glm::vec3(300.1f, 300.1f, 300.1f));	 // �̹�

	//set up floating point framebuffer to render scene;
	GLuint hdrFBO;
	glGenFramebuffers(1, &hdrFBO);
	//-Create floating point color buffer
	GLuint colorBuffer;
	glGenTextures(1, &colorBuffer);
	glBindTexture(GL_TEXTURE_2D, colorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	//-Create depth buffer(renderBuffer)
	GLuint rboDepthBuffer;
	glGenRenderbuffers(1, &rboDepthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WINDOW_WIDTH, WINDOW_HEIGHT);
	
	//attach buffers
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepthBuffer);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		return 0;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	
	BoundingBox box;
	int nVal = sizeof(cubeVertices2) / sizeof(GLfloat);
	for (int i = 0; i < nVal; i += 8)
	{
		glm::vec3 pnt(cubeVertices2[i], cubeVertices2[i + 1], cubeVertices2[i + 2]);
		box.Merge(pnt);
	}
	
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 25.0));
	model = glm::scale(model, glm::vec3(2.5f, 2.5f, 27.5f));
	BoundingBox totalBoundingBox = box.Transformed(model);

	camera.InitCamera(totalBoundingBox,0.3f);

	// Section2 ׼����ɫ������
	Shader shader("lighting.vertex", "lighting.frag");
	Shader shaderCube("light_cube.vertex", "light_cube.frag");
	Shader shaderHdr("hdrExposure.vertex", "hdrExposure.frag");

	shader.use();
	glm::mat4 projectionMatrix = camera.GetProjectionMatrix((float)WINDOW_WIDTH / (float)WINDOW_HEIGHT);
	shader.setMat4("projection", projectionMatrix);
	shader.setMat4("model", model);
	shader.setInt("diffuseMap", 0);
	for (auto i = 0; i < lightPositions.size(); i++)
	{
		shader.setVec3("pointlights[" + std::to_string(i) + "].position", lightPositions[i]);
		shader.setVec3("pointlights[" + std::to_string(i) + "].color", lightColors[i]);
	}
	shader.setBool("inverse_normals", true);
	shader.unUse();
	
	shaderCube.use();
	shaderCube.setMat4("projection", projectionMatrix);
	shaderCube.unUse();

	

	int nVertex = sizeof(cubeVertices2) / (sizeof(GLfloat) * 8);
	
	// ��ʼ��Ϸ��ѭ��
	glEnable(GL_DEPTH_TEST);
	glm::vec3 targetPos = totalBoundingBox.GetCenter();

	
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame; // ��һ֡��ʱ��
		processInput(window, camera);
		glfwPollEvents(); // ����������� ���̵��¼�

		glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
		// �����ɫ������ ����Ϊָ����ɫ
		glClearColor(0.18f, 0.04f, 0.14f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		// ������д�������ƴ���
		glBindVertexArray(VAOId);
		float distance = glm::length(targetPos - camera.Position);
		targetPos = camera.Position + distance * camera.Front;
		glm::mat4 viewMatrix = camera.GetViewMatrix(targetPos);
		shader.use();
		//����ƶ�����ͷ���򲻱�
		shader.setMat4("view", viewMatrix);
	
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, woodTexId);

		glDrawArrays(GL_TRIANGLES, 0, nVertex);
		shader.unUse();

		shaderCube.use();
		shaderCube.setMat4("view", viewMatrix);
		for (auto i = 0; i < lightPositions.size(); i++)
		{
			glm::mat4 lightModelMatrix(1.0f);
			lightModelMatrix = glm::translate(lightModelMatrix, lightPositions[i]);
			lightModelMatrix = glm::scale(lightModelMatrix, glm::vec3(0.2, 0.2, 0.2));
			shaderCube.setMat4("model", lightModelMatrix);
			shaderCube.setVec3("lightColor", lightColors[i]);
			glDrawArrays(GL_TRIANGLES, 0, nVertex);
		}
		shaderCube.unUse();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.f, 0.f, 0.f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindVertexArray(quadVAOId);

		shaderHdr.use();
		shaderHdr.setBool("enableHdrFlg", hdrFlg);
		//cout<<"hdrFlg:"<<hdrFlg<<endl;
		std::cout << "hdr: " << (hdrFlg ? "on" : "off") << "| exposure: " << exposureValue << std::endl;

		shaderHdr.setInt("colorBuffer", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorBuffer);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, squareIndexes);
		shaderHdr.unUse();
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
//void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
//{
//	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
//	{
//		glfwSetWindowShouldClose(window, GL_TRUE); // �رմ���
//	}
//}
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
	if (glfwGetKey(ptrWindow, GLFW_KEY_SPACE) == GLFW_PRESS )
	{
		hdrFlg = !hdrFlg;
		
	}
	if (glfwGetKey(ptrWindow, GLFW_KEY_SPACE) == GLFW_RELEASE)
	{
		hdrKeyPressed = false;
	}

	if (glfwGetKey(ptrWindow, GLFW_KEY_Q) == GLFW_PRESS)
	{
		if (exposureValue > 0.0f)
			exposureValue -= 0.001f;
		else
			exposureValue = 0.0f;
	}
	else if (glfwGetKey(ptrWindow, GLFW_KEY_E) == GLFW_PRESS)
	{
		exposureValue += 0.001f;
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