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
#include "learnopengl/Maroc.h"
#include <thread>

// ���̻ص�����ԭ������
void processInput(GLFWwindow* window, Camera& camera);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);// ���������
void UpdateInstanceMatrix(vector<glm::mat4>& vecInsMatrix);
void UpdateInstanceColor(vector<glm::vec3>& vecInsColor);
const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;
const int AMOUNT = 10000;
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
	std::cout << "Start OpenGL core profile version 4.5" << std::endl;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
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
	GL_CHECK_ERROR; //GLEW��һ����ʷ�ƾõ�bug������glewInit()������һ��GL_INVALID_ENUM�Ĵ����ǣ�
	                //���Ե�һ�ε��õ�glGetError��Զ��⧲������ظ��㷵��һ��������롣
					//���Ҫ�޸����bug�����ǽ������ڵ���glewInit֮����������glGetError���������ǣ�
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

	vecCubeModelMatrix.reserve(AMOUNT);

	vector<glm::vec3>vecCubeModelColor;
	vecCubeModelColor.reserve(AMOUNT);
	UpdateInstanceMatrix(vecCubeModelMatrix);
	UpdateInstanceColor(vecCubeModelColor);

	

	BoundingBox totalCubeBoundingBox;
	for (auto& modelMatrix : vecCubeModelMatrix)
	{
		BoundingBox tempBox = box.Transformed(modelMatrix);
		totalCubeBoundingBox.Merge(tempBox);
	}
	camera.InitCamera(totalCubeBoundingBox, 0.8f);
	
	// Section2 ׼����ɫ������
	Shader sceneShader("scene.vertex", "scene.frag");
	glm::mat4 viewMatrix = camera.GetViewMatrix();
	glm::mat4 projection = camera.GetProjectionMatrix((GLfloat)(WINDOW_WIDTH) / (GLfloat)(WINDOW_HEIGHT));

	sceneShader.use();
	sceneShader.setMat4("view", viewMatrix);
	sceneShader.setMat4("projection", projection);
	sceneShader.unUse();

	GLuint one_query;
	glGenQueries(1, &one_query);
	GL_INPUT_ERROR

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
		

		// ������д�������ƴ���
		vaoBuffer.Bind();

		sceneShader.use();
		int nOcclusionCount = 0;
		for (auto i = 0; i < AMOUNT; i++)
		{
			sceneShader.setMat4("model", vecCubeModelMatrix[i]);
			sceneShader.setVec3("color", vecCubeModelColor[i]);
			glBeginQuery(GL_SAMPLES_PASSED, one_query);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glEndQuery(GL_SAMPLES_PASSED);
			GLuint the_result = 0;
			glGetQueryObjectuiv(one_query, GL_QUERY_RESULT, &the_result);
			if (!the_result)
			{
				//cout << "no no no " << endl;
				++nOcclusionCount;
			}
		}
		cout << "nOcclusionCount = " << nOcclusionCount << endl;
		
		sceneShader.unUse();
		glBindVertexArray(0);
		glfwSwapBuffers(window); // ��������
		int span = (int)((glfwGetTime() - currentFrame)*1000);
		char str[100] = {};
		//sprintf(str, "��������%dms", span);
		cout << span << endl;

	}
	// �ͷ���Դ
	vaoBuffer.DeleteBuffer();
	glDeleteQueries(1, &one_query);
	one_query = 0;
	sceneShader.deleteProgram();
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
		float displacement = (rand() % (int)(2 * offset * AMOUNT)) / (float)AMOUNT - offset;
		float x = sin(angle) * radius + displacement;
		displacement = (rand() % (int)(2 * offset * AMOUNT)) / (float)AMOUNT - offset;
		float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
		displacement = (rand() % (int)(2 * offset * AMOUNT)) / (float)AMOUNT - offset;
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
		GLfloat rColor = ((rand() % AMOUNT) / 200.0f) + 0.5; // Between 0.5 and 1.0
		GLfloat gColor = ((rand() % AMOUNT) / 200.0f) + 0.5; // Between 0.5 and 1.0
		GLfloat bColor = ((rand() % AMOUNT) / 200.0f) + 0.5; // Between 0.5 and 1.0
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