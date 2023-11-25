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
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
// ���̻ص�����ԭ������
void processInput(GLFWwindow* window, Camera& camera);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// ���������
const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;
float deltaTime = 0.0f; // ��ǰ֡����һ֡��ʱ���
float lastFrame = 0.0f; // ��һ֡��ʱ��
float lastX = WINDOW_WIDTH / 2.0f;
float lastY = WINDOW_HEIGHT / 2.0f;
bool  bFirstMove = true;
GLuint cubeVAOId = 0;
GLuint cubeVBOId = 0;
Camera camera;
void RenderScene(const Shader& shader);
void RenderCube();
void RenderLight(const Shader& shader, glm::mat4&modelMatrix);

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
#pragma region imgui��ʼ��
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();//������ɫ
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");
#pragma endregion

	//׼������
	GLuint woodTexId = TextureFromFile("wood.png", "../resources/textures");
	//configure depth map FBO
	const unsigned int SHADOW_WIDTH = 1024;
	const unsigned int SHADOW_HEIGTH = 1024;
	//���������ͼFBO
	
	GLuint depthCubeMap;
	glGenTextures(1, &depthCubeMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);
	for (GLuint i = 0; i < 6; i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
			SHADOW_WIDTH, SHADOW_HEIGTH,
			0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	

	GLuint depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO); 
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubeMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	// Section2 ׼����ɫ������
	Shader shadowMappingShader("point_shadow_mapping.vertex", "point_shadow_mapping.frag");
	Shader depthMapShader("point_shadow_mapping_depth.vertex", "point_shadow_mapping_depth.frag", "point_shadow_mapping_depth.gs");
	Shader lightShader("light_cube.vertex", "light_cube.frag");
	
	shadowMappingShader.use();
	shadowMappingShader.setInt("diffuseTexture", 0);
	shadowMappingShader.setInt("depthMap", 1);
	shadowMappingShader.unUse();
	glm::vec3 lightPos = glm::vec3(0.f,0.f,0.f);
	
	GLfloat aspect = (GLfloat)(SHADOW_WIDTH) / (GLfloat)(SHADOW_HEIGTH);
	float near_plane = 1.0f;
	float far_plane = 25.0f;
	
	glm::mat4 lightProjectionMatrix = glm::perspective(glm::radians(90.f), aspect, near_plane, far_plane);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	// ��ʼ��Ϸ��ѭ��
	while (!glfwWindowShouldClose(window))
	{
		processInput(window, camera);
		glfwPollEvents(); // ����������� ���̵��¼�
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame; // ��һ֡��ʱ��
		
		lightPos.z = 3.f * glm::sin(currentFrame)*0.5f;
		
		vector<glm::mat4> vecLightPVMatrix;
		vecLightPVMatrix.emplace_back(lightProjectionMatrix * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f,  0.f,   0.f), glm::vec3(0.0f, -1.0f, 0.0f)));
		vecLightPVMatrix.emplace_back(lightProjectionMatrix * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.f, 0.f), glm::vec3(0.0f, -1.0f, 0.0f)));
		vecLightPVMatrix.emplace_back(lightProjectionMatrix * glm::lookAt(lightPos, lightPos + glm::vec3(0.f, 1.0f, 0.f), glm::vec3(0.0f, 0.0f, 1.0f)));
		vecLightPVMatrix.emplace_back(lightProjectionMatrix * glm::lookAt(lightPos, lightPos + glm::vec3(0.f, -1.0f, 0.f), glm::vec3(0.0f, 0.0f, -1.0f)));
		vecLightPVMatrix.emplace_back(lightProjectionMatrix * glm::lookAt(lightPos, lightPos + glm::vec3(0.f, 0.f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		vecLightPVMatrix.emplace_back(lightProjectionMatrix * glm::lookAt(lightPos, lightPos + glm::vec3(0.f, 0.f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		// �����ɫ������ ����Ϊָ����ɫ
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		//�û���������framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGTH);
		// ������д�������ƴ���
		depthMapShader.use();
		for (int i = 0; i < 6; i++)
		{
			depthMapShader.setMat4("shadowMatrices[" + to_string(i) + "]", vecLightPVMatrix[i]);
		}
		depthMapShader.setVec3("lightPos", lightPos);
		depthMapShader.setFloat("far_plane", far_plane);
		RenderScene(depthMapShader);
		depthMapShader.unUse();
		//ϵͳĬ�ϵ�framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// reset viewport
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

		shadowMappingShader.use();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		shadowMappingShader.setMat4("projection", projection);
		shadowMappingShader.setMat4("view", view);
		// set lighting uniforms
		shadowMappingShader.setVec3("lightPos", lightPos);
		shadowMappingShader.setVec3("viewPos", camera.Position);
		
		shadowMappingShader.setFloat("far_plane", far_plane);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, woodTexId);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);
		RenderScene(shadowMappingShader);
		shadowMappingShader.unUse();

		/*lightShader.use();
		lightShader.setMat4("view", camera.GetViewMatrix());
		lightShader.setMat4("projection", projection);
		glm::mat4 lightModelMatrix(1.0);
		lightModelMatrix = glm::scale(lightModelMatrix, glm::vec3(0.15f, 0.15f, 0.15));
		lightModelMatrix = glm::translate(lightModelMatrix, glm::vec3(lightPos));
		RenderLight(lightShader,lightModelMatrix);
		lightShader.unUse();*/
#pragma region create element
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::Begin("Hello Gamma");
		
		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#pragma endregion
		
		
		glfwSwapBuffers(window); // ��������
	}
	// �ͷ���Դ
	glDeleteVertexArrays(1, &cubeVAOId);	
	glDeleteBuffers(1, &cubeVBOId);

#pragma region Cleanup imgui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
#pragma endregion
	glfwTerminate();
	return 0;
}

void RenderScene(const Shader& shader)
{
	// room cube
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(5.0f));
	shader.setMat4("model", model);
	glDisable(GL_CULL_FACE); // note that we disable culling here since we render 'inside' the cube instead of the usual 'outside' which throws off the normal culling methods.
	shader.setInt("reverse_normals", 1); // A small little hack to invert normals when drawing cube from the inside so lighting still works.
	RenderCube();
	shader.setInt("reverse_normals", 0); // and of course disable it
	glEnable(GL_CULL_FACE);
	// cubes
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(4.0f, -3.5f, 0.0));
	model = glm::scale(model, glm::vec3(0.5f));
	shader.setMat4("model", model);
	RenderCube();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(2.0f, 3.0f, 1.0));
	model = glm::scale(model, glm::vec3(0.75f));
	shader.setMat4("model", model);
	RenderCube();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-3.0f, -1.0f, 0.0));
	model = glm::scale(model, glm::vec3(0.5f));
	shader.setMat4("model", model);
	RenderCube();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.5f, 1.0f, 1.5));
	model = glm::scale(model, glm::vec3(0.5f));
	shader.setMat4("model", model);
	RenderCube();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.5f, 2.0f, -3.0));
	model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	model = glm::scale(model, glm::vec3(0.75f));
	shader.setMat4("model", model);
	RenderCube();

}

void RenderCube()
{
	
	if (cubeVAOId == 0)
	{
		// ����cube�������
		vector<vertex_attribute> vecVertexAttrib;
		map< vertex_attribute, int> mapVertexAttrib2Num;
		vecVertexAttrib.push_back(vertex_attribute::position);
		vecVertexAttrib.push_back(vertex_attribute::normal);
		vecVertexAttrib.push_back(vertex_attribute::texcoord);
		mapVertexAttrib2Num[vertex_attribute::position] = 3;
		mapVertexAttrib2Num[vertex_attribute::normal] = 3;
		mapVertexAttrib2Num[vertex_attribute::texcoord] = 2;
		VAOBuffer cubeVaoBuffer;
		cubeVaoBuffer.BuildVAO(cubeVertices2, sizeof(cubeVertices2),
			nullptr, 0, vecVertexAttrib, mapVertexAttrib2Num);
		cubeVAOId = cubeVaoBuffer.GetVAO();
		cubeVBOId = cubeVaoBuffer.GetVAO();
	}
	// render Cube
	glBindVertexArray(cubeVAOId);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void RenderLight(const Shader& shader, glm::mat4&modelMatrix)
{
	shader.setMat4("model", modelMatrix);
	glDrawArrays(GL_TRIANGLES, 0, 36);
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