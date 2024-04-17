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
#include "learnopengl/boundingbox.h"
#include "learnopengl/model.h"
#include "learnopengl/camera.h"
#include "learnopengl/vertexset.h"

// ���̻ص�����ԭ������
//void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow* window, Camera& camera);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
glm::mat4 calculate_model_matrix(const glm::vec3& position, const glm::vec3& rotation = glm::vec3(0.f) , const glm::vec3& scale=glm::vec3(1.f));
//
// ���������
const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;
float deltaTime = 0.0f; // ��ǰ֡����һ֡��ʱ���
float lastFrame = 0.0f; // ��һ֡��ʱ��
float lastX = WINDOW_WIDTH / 2.0f;
float lastY = WINDOW_HEIGHT / 2.0f;
bool  bFirstMove = true;
Camera camera(glm::vec3(0.f,0.f,5.0f));
int main(int argc, char** argv)
{

	if (!glfwInit())	// ��ʼ��glfw��
	{
		std::cout << "Error::GLFW could not initialize GLFW!" << std::endl;
		return -1;
	}

	// ����OpenGL 3.3 core profile
	std::cout << "Start OpenGL core profile version 4.2" << std::endl;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); //��Ҫ����opengl �İ汾 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// ��������
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
		"Demo of blend", NULL, NULL);
	if (!window)
	{
		std::cout << "Error::GLFW could not create winddow!" << std::endl;
		glfwTerminate();
		return -1;
	}
	// �����Ĵ��ڵ�contextָ��Ϊ��ǰcontext
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
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

	

	// build and compile shaders
	Shader solidShader("Solid.vertex", "Solid.frag");
	Shader transparentShader("Transprent.vertex", "Transprent.frag");
	Shader compositeShader("Composite.vertex", "Composite.frag");
	Shader screenShader("Screen.vertex", "Screen.frag");
	//// �����ӿڲ���
	//glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	////����͸��GPU ����
	//VAOBuffer vaoBuffer;
	//vector<vertex_attribute> vecAttrib;
	//map<vertex_attribute, int> mapAttrib2Size;
	//vecAttrib.emplace_back(vertex_attribute::position);
	//vecAttrib.emplace_back(vertex_attribute::texcoord);
	//mapAttrib2Size[vertex_attribute::position] = 3;
	//mapAttrib2Size[vertex_attribute::texcoord] = 2;
	//
	//vaoBuffer.BuildVAO(squareVertices, sizeof(squareVertices), squareIndexes,
	//	sizeof(squareIndexes), vecAttrib, mapAttrib2Size);

	//// �����������
	//GLuint vaoId = vaoBuffer.GetVAO();
	//GLuint vboId = vaoBuffer.GetVBO();
	//// set up framebuffers and their texture attachments
	//// ------------------------------------------------------------------
	//unsigned int opaqueFBO, transparentFBO;
	//glGenFramebuffers(1, &opaqueFBO);
	//glGenFramebuffers(1, &transparentFBO);

	//// set up attachments for opaque framebuffer
	//unsigned int opaqueTexture;
	//glGenTextures(1, &opaqueTexture);
	//glBindTexture(GL_TEXTURE_2D, opaqueTexture);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_HALF_FLOAT, NULL);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glBindTexture(GL_TEXTURE_2D, 0);

	//unsigned int depthTexture;
	//glGenTextures(1, &depthTexture);
	//glBindTexture(GL_TEXTURE_2D, depthTexture);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	//glBindTexture(GL_TEXTURE_2D, 0);

	//glBindFramebuffer(GL_FRAMEBUFFER, opaqueFBO);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, opaqueTexture, 0);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
	//
	//if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	//	std::cout << "ERROR::FRAMEBUFFER:: Opaque framebuffer is not complete!" << std::endl;

	//glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//// set up attachments for transparent framebuffer
	//unsigned int accumTexture;
	//glGenTextures(1, &accumTexture);
	//glBindTexture(GL_TEXTURE_2D, accumTexture);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_HALF_FLOAT, NULL);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glBindTexture(GL_TEXTURE_2D, 0);

	//unsigned int revealTexture;
	//glGenTextures(1, &revealTexture);
	//glBindTexture(GL_TEXTURE_2D, revealTexture);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glBindTexture(GL_TEXTURE_2D, 0);

	//glBindFramebuffer(GL_FRAMEBUFFER, transparentFBO);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, accumTexture, 0);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, revealTexture, 0);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);// opaque framebuffer's depth texture

	//const GLenum transparentDrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	//if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	//	std::cout << "ERROR::FRAMEBUFFER:: Transparent framebuffer is not complete!" << std::endl;

	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//// set up transformation matrices
	//// ------------------------------------------------------------------
	//glm::mat4 redModelMtx = calculate_model_matrix(glm::vec3(0.f, 0.f, 1.f));
	//glm::mat4 greenModelMtx = calculate_model_matrix(glm::vec3(0.f, 0.f, 0.f));
	//glm::mat4 blueModelMtx = calculate_model_matrix(glm::vec3(0.f, 0.f, 2.f));

	//// set up intermediate variables
	//// ------------------------------------------------------------------
	//glm::vec4 zeroFillerVec(0.0f);
	//glm::vec4 oneFillerVec(1.0f);
	//// ��ʼ��Ϸ��ѭ��
	//
	//
	//
	//while (!glfwWindowShouldClose(window))
	//{
	//	float currentFrame = glfwGetTime();
	//	deltaTime = currentFrame - lastFrame;
	//	lastFrame = currentFrame; // ��һ֡��ʱ��
	//	processInput(window, camera);
	//	glfwPollEvents(); // ����������� ���̵��¼�


	//	//camera matrices 
	//	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)(WINDOW_WIDTH) / WINDOW_HEIGHT, 0.1f, 100.f);
	//	glm::mat4 view = camera.GetViewMatrix();
	//	glm::mat4 vp = projection * view;
	//	glEnable(GL_DEPTH_TEST);
	//	glDepthFunc(GL_LESS);
	//	glDepthMask(true);
	//	glDisable(GL_BLEND);
	//	// �����ɫ������ ����Ϊָ����ɫ
	//	glClearColor(0.f, 0.f, 0.f, 0.0f);
	//	glBindFramebuffer(GL_FRAMEBUFFER, opaqueFBO);
	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//	solidShader.use();
	//	glm::mat4 mvp = vp * redModelMtx;
	//	solidShader.setVec3("color", glm::vec3(1.f, 0.f, 0.f));
	//	solidShader.setMat4("mvp", mvp);
	//	vaoBuffer.Bind();
	//	//���ʹ�������ķ�ʽ�������������EBO
	//	vaoBuffer.BindEBO();
	//	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	//	solidShader.unUse();
	//	// draw transparent objects (transparent pass)
	//	// -----

	//	// configure render states
	//	glDepthMask(GL_FALSE);
	//	glEnable(GL_BLEND);
	//	glBlendFunci(0, GL_ONE, GL_ONE);
	//	glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
	//	glBlendEquation(GL_FUNC_ADD);
	//	// bind transparent framebuffer to render transparent objects
	//	glBindFramebuffer(GL_FRAMEBUFFER, transparentFBO);
	//	glClearBufferfv(GL_COLOR, 0, &zeroFillerVec[0]);
	//	glClearBufferfv(GL_COLOR, 1, &oneFillerVec[0]);

	//	//use transparent shader
	//	transparentShader.use();
	//	//draw green quad
	//	mvp = vp * greenModelMtx;
	//	transparentShader.setMat4("mvp", mvp);
	//	transparentShader.setVec4("color",glm::vec4(0.0f,1.0f,0.0f,0.5f));
	//	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	//	// draw blue quad
	//	mvp = vp * blueModelMtx;
	//	transparentShader.setMat4("mvp", mvp);
	//	transparentShader.setVec4("color", glm::vec4(0.0f, 1.0f, 0.0f, 0.5f));
	//	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	//	transparentShader.unUse();

	//	// draw composite image (composite pass)
	//	// -----

	//	// set render states
	//	glDepthFunc(GL_ALWAYS);
	//	glEnable(GL_BLEND);
	//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//	// bind opaque framebuffer
	//	glBindFramebuffer(GL_FRAMEBUFFER, opaqueFBO);

	//	// use composite shader
	//	compositeShader.use();

	//	// draw screen quad
	//	glActiveTexture(GL_TEXTURE0);
	//	glBindTexture(GL_TEXTURE_2D, accumTexture);
	//	glActiveTexture(GL_TEXTURE1);
	//	glBindTexture(GL_TEXTURE_2D, revealTexture);
	//
	//	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


	//	// draw to backbuffer (final pass)
	//	// -----

	//	// set render states
	//	glDisable(GL_DEPTH_TEST);
	//	glDepthMask(GL_TRUE); // enable depth writes so glClear won't ignore clearing the depth buffer
	//	glDisable(GL_BLEND);

	//	// bind backbuffer
	//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	//	// use screen shader
	//	screenShader.use();

	//	// draw final screen quad
	//	glActiveTexture(GL_TEXTURE0);
	//	glBindTexture(GL_TEXTURE_2D, opaqueTexture);
	//	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	//	glBindVertexArray(0);
	//	glUseProgram(0);

	//	glfwSwapBuffers(window); // ��������
	//}
	//// �ͷ���Դ
	////glDeleteVertexArrays(1, &transparentVAOId);
	////glDeleteBuffers(1, &transparentVBOId);
	//glfwTerminate();
// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
float quadVertices[] = {
	// positions		// uv
	-1.0f, -1.0f, 0.0f,	0.0f, 0.0f,
	 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,

	 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
	-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
	-1.0f, -1.0f, 0.0f, 0.0f, 0.0f
};

// quad VAO
unsigned int quadVAO, quadVBO;
glGenVertexArrays(1, &quadVAO);
glGenBuffers(1, &quadVBO);
glBindVertexArray(quadVAO);
glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
glEnableVertexAttribArray(0);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
glEnableVertexAttribArray(1);
glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
glBindVertexArray(0);

// set up framebuffers and their texture attachments
// ------------------------------------------------------------------
unsigned int opaqueFBO, transparentFBO;
glGenFramebuffers(1, &opaqueFBO);
glGenFramebuffers(1, &transparentFBO);

// set up attachments for opaque framebuffer
unsigned int opaqueTexture;
glGenTextures(1, &opaqueTexture);
glBindTexture(GL_TEXTURE_2D, opaqueTexture);
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_HALF_FLOAT, NULL);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glBindTexture(GL_TEXTURE_2D, 0);

unsigned int depthTexture;
glGenTextures(1, &depthTexture);
glBindTexture(GL_TEXTURE_2D, depthTexture);
glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
glBindTexture(GL_TEXTURE_2D, 0);

glBindFramebuffer(GL_FRAMEBUFFER, opaqueFBO);
glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, opaqueTexture, 0);
glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
std::cout << "ERROR::FRAMEBUFFER:: Opaque framebuffer is not complete!" << std::endl;

glBindFramebuffer(GL_FRAMEBUFFER, 0);

// set up attachments for transparent framebuffer
unsigned int accumTexture;
glGenTextures(1, &accumTexture);
glBindTexture(GL_TEXTURE_2D, accumTexture);
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_HALF_FLOAT, NULL);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glBindTexture(GL_TEXTURE_2D, 0);

unsigned int revealTexture;
glGenTextures(1, &revealTexture);
glBindTexture(GL_TEXTURE_2D, revealTexture);
glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glBindTexture(GL_TEXTURE_2D, 0);

glBindFramebuffer(GL_FRAMEBUFFER, transparentFBO);
glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, accumTexture, 0);
glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, revealTexture, 0);
glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0); // opaque framebuffer's depth texture

const GLenum transparentDrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
glDrawBuffers(2, transparentDrawBuffers);

if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
std::cout << "ERROR::FRAMEBUFFER:: Transparent framebuffer is not complete!" << std::endl;

glBindFramebuffer(GL_FRAMEBUFFER, 0);

// set up transformation matrices
// ------------------------------------------------------------------
glm::mat4 redModelMat = calculate_model_matrix(glm::vec3(0.0f, -0.5f, 0.0f));
glm::mat4 greenModelMat = calculate_model_matrix(glm::vec3(0.0f, 1.0f, 1.0f));
glm::mat4 blueModelMat = calculate_model_matrix(glm::vec3(0.0f, 0.0f, 2.0f));

// set up intermediate variables
// ------------------------------------------------------------------
glm::vec4 zeroFillerVec(0.0f);
glm::vec4 oneFillerVec(1.0f);

// render loop
// -----------
while (!glfwWindowShouldClose(window))
{
	// per-frame time logic
	// --------------------
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	// camera matrices
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 vp = projection * view;

	// input
	// -----
	processInput(window,camera);

	// render
	// ------

	// draw solid objects (solid pass)
	// ------

	// configure render states
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// bind opaque framebuffer to render solid objects
	glBindFramebuffer(GL_FRAMEBUFFER, opaqueFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// use solid shader
	solidShader.use();

	// draw red quad
	solidShader.setMat4("mvp", vp * redModelMat);
	solidShader.setVec3("color", glm::vec3(1.0f, 0.0f, 0.0f));
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// draw transparent objects (transparent pass)
	// -----

	// configure render states
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunci(0, GL_ONE, GL_ONE);
	glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
	glBlendEquation(GL_FUNC_ADD);

	// bind transparent framebuffer to render transparent objects
	glBindFramebuffer(GL_FRAMEBUFFER, transparentFBO);
	glClearBufferfv(GL_COLOR, 0, &zeroFillerVec[0]);
	glClearBufferfv(GL_COLOR, 1, &oneFillerVec[0]);

	// use transparent shader
	transparentShader.use();

	// draw green quad
	transparentShader.setMat4("mvp", vp * greenModelMat);
	transparentShader.setVec4("color", glm::vec4(0.0f, 1.0f, 0.0f, 0.5f));
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// draw blue quad
	transparentShader.setMat4("mvp", vp * blueModelMat);
	transparentShader.setVec4("color", glm::vec4(0.0f, 0.0f, 1.0f, 0.5f));
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// draw composite image (composite pass)
	// -----

	// set render states
	glDepthFunc(GL_ALWAYS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// bind opaque framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, opaqueFBO);

	// use composite shader
	compositeShader.use();

	// draw screen quad
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, accumTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, revealTexture);
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// draw to backbuffer (final pass)
	// -----

	// set render states
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE); // enable depth writes so glClear won't ignore clearing the depth buffer
	glDisable(GL_BLEND);

	// bind backbuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// use screen shader
	screenShader.use();

	// draw final screen quad
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, opaqueTexture);
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
	// -------------------------------------------------------------------------------
	glfwSwapBuffers(window);
	glfwPollEvents();
}

// optional: de-allocate all resources once they've outlived their purpose:
// ------------------------------------------------------------------------
glDeleteVertexArrays(1, &quadVAO);
glDeleteBuffers(1, &quadVBO);
glDeleteTextures(1, &opaqueTexture);
glDeleteTextures(1, &depthTexture);
glDeleteTextures(1, &accumTexture);
glDeleteTextures(1, &revealTexture);
glDeleteFramebuffers(1, &opaqueFBO);
glDeleteFramebuffers(1, &transparentFBO);

glfwTerminate();

return EXIT_SUCCESS;
	return 0;
}
//void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
//{
//	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
//	{
//		glfwSetWindowShouldClose(window, GL_TRUE); // �رմ���
//	}
//}
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
// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}
// generate a model matrix
// ---------------------------------------------------------------------------------------------------------
glm::mat4 calculate_model_matrix(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
{
	glm::mat4 trans = glm::mat4(1.0f);

	trans = glm::translate(trans, position);
	trans = glm::rotate(trans, glm::radians(rotation.x), glm::vec3(1.0, 0.0, 0.0));
	trans = glm::rotate(trans, glm::radians(rotation.y), glm::vec3(0.0, 1.0, 0.0));
	trans = glm::rotate(trans, glm::radians(rotation.z), glm::vec3(0.0, 0.0, 1.0));
	trans = glm::scale(trans, scale);

	return trans;
}