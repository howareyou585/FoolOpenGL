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
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
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
float exposure = 1.0f;
int amount = 10; // �����һ����һ��ż����ˮƽһ�� ��ֱһ�Ρ�
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
		"Demo of bloom", NULL, NULL);
	if (!window)
	{
		std::cout << "Error::GLFW could not create winddow!" << std::endl;
		glfwTerminate();
		return -1;
	}
	// �����Ĵ��ڵ�contextָ��Ϊ��ǰcontext
	glfwMakeContextCurrent(window);

#pragma region imgui��ʼ��
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();//������ɫ
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");
#pragma endregion


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
	
#pragma region  ����CubeVAOBuffer
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
#pragma endregion

#pragma region ����quad VAOBuffer
	VAOBuffer quadVaoBuffer;
	
	vecAttrib.clear();
	mapAttrib2Size.clear();
	vecAttrib.emplace_back(vertex_attribute::position);
	vecAttrib.emplace_back(vertex_attribute::texcoord);
	mapAttrib2Size[vertex_attribute::position] = 2;
	mapAttrib2Size[vertex_attribute::texcoord] = 2;
	quadVaoBuffer.BuildVAO(quadVertices, sizeof(quadVertices), nullptr,
		0, vecAttrib, mapAttrib2Size);

	GLuint quadVAO = quadVaoBuffer.GetVAO();
	GLuint quadVBO = quadVaoBuffer.GetVBO();
#pragma endregion

	
#pragma region ������ͼ
	//������ͼ
	GLuint woodTexId = TextureFromFile("wood.png", "../resources/textures");
	GLuint containTexId = TextureFromFile("container2.png", "../resources/textures"); 
#pragma endregion

	
#pragma region MyRegion
	// lighting info
   // -------------
   // positions
	std::vector<glm::vec3> lightPositions;
	lightPositions.push_back(glm::vec3(0.0f, 0.5f, 1.5f));
	lightPositions.push_back(glm::vec3(-4.0f, 0.5f, -3.0f));
	lightPositions.push_back(glm::vec3(3.0f, 0.5f, 1.0f));
	lightPositions.push_back(glm::vec3(-.8f, 2.4f, -1.0f));
	// colors�� ����RGB��������1�����
	std::vector<glm::vec3> lightColors;
	lightColors.push_back(glm::vec3(5.0f, 5.0f, 5.0f));
	lightColors.push_back(glm::vec3(10.0f, 0.0f, 0.0f));
	lightColors.push_back(glm::vec3(0.0f, 0.0f, 15.0f));
	lightColors.push_back(glm::vec3(0.0f, 5.0f, 0.0f));
#pragma endregion

#pragma region ����HDRFBO
	//set up floating point framebuffer to render scene;
	GLuint hdrFBO;
	glGenFramebuffers(1, &hdrFBO);
	//-Create floating point color buffer
	GLuint hdrColorBuffer[2]; 
	glGenTextures(2, hdrColorBuffer);
	for (auto i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_2D, hdrColorBuffer[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	}
	//-Create depth buffer(renderBuffer)
	GLuint rboDepthBuffer;
	glGenRenderbuffers(1, &rboDepthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WINDOW_WIDTH, WINDOW_HEIGHT);

	//attach color buffers 1 and color buffer2
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	for (int i = 0; i < 2; i++)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_TEXTURE_2D, hdrColorBuffer[i], 0);
	}
	//attach depth buffers 
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepthBuffer);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		return 0;
	}
	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
#pragma endregion
#pragma region ����TWO BLUR FBO 
	GLuint blurFBO[2]={};  // ˮƽ���� + ��ֱ���� ģ��FBO
	GLuint blurColorBuffer[2] = {}; 
	glGenFramebuffers(2, blurFBO);
	glGenTextures(2, blurColorBuffer);
	for (auto i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_2D, blurColorBuffer[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glBindFramebuffer(GL_FRAMEBUFFER, blurFBO[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurColorBuffer[i], 0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			return 0;
		}
	}
	
#pragma endregion


	BoundingBox box;
	int nVal = sizeof(cubeVertices2) / sizeof(GLfloat);
	for (int i = 0; i < nVal; i += 8)
	{
		glm::vec3 pnt(cubeVertices2[i], cubeVertices2[i + 1], cubeVertices2[i + 2]);
		box.Merge(pnt);
	}
#pragma region  ������Cube��λ��
	vector<glm::mat4> vecCubeModelMtx;
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0));
	model = glm::scale(model, glm::vec3(12.5f, 0.5f, 12.5f));
	vecCubeModelMtx.emplace_back(model);
	BoundingBox modelBoundingBox = box.Transformed(model);
	BoundingBox totalBoundingBox = modelBoundingBox;

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
	model = glm::scale(model, glm::vec3(0.5f));
	vecCubeModelMtx.emplace_back(model);
	modelBoundingBox = box.Transformed(model);
	totalBoundingBox.Merge(modelBoundingBox);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
	model = glm::scale(model, glm::vec3(0.5f));
	vecCubeModelMtx.emplace_back(model);
	modelBoundingBox = box.Transformed(model);
	totalBoundingBox.Merge(modelBoundingBox);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 2.0));
	model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	vecCubeModelMtx.emplace_back(model);
	modelBoundingBox = box.Transformed(model);
	totalBoundingBox.Merge(modelBoundingBox);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 2.7f, 4.0));
	model = glm::rotate(model, glm::radians(23.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	model = glm::scale(model, glm::vec3(1.25));
	vecCubeModelMtx.emplace_back(model);
	modelBoundingBox = box.Transformed(model);
	totalBoundingBox.Merge(modelBoundingBox);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-2.0f, 1.0f, -3.0));
	model = glm::rotate(model, glm::radians(124.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	vecCubeModelMtx.emplace_back(model);
	modelBoundingBox = box.Transformed(model);
	totalBoundingBox.Merge(modelBoundingBox);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-3.0f, 0.0f, 0.0));
	model = glm::scale(model, glm::vec3(0.5f));
	vecCubeModelMtx.emplace_back(model);
	modelBoundingBox = box.Transformed(model);
	totalBoundingBox.Merge(modelBoundingBox);
#pragma endregion

	camera.InitCamera(totalBoundingBox,0.3f);

	// Section2 ׼����ɫ������
	Shader preBloomShader("PrepareBloom.vertex", "PrepareBloom.frag");
	Shader lightBoxShader("PrepareBloom.vertex", "light_box.frag");
	Shader blurShader("Blur.vertex", "Blur.frag");
	Shader BloomShader("Bloom.vertex", "Bloom.frag");
	
	
	BloomShader.use();
	BloomShader.setInt("scene", 0);
	BloomShader.setInt("bloomBlur", 1);
	BloomShader.unUse();

	preBloomShader.use();
	
	glm::mat4 projectionMatrix = camera.GetProjectionMatrix((float)WINDOW_WIDTH / (float)WINDOW_HEIGHT);
	preBloomShader.setMat4("projection", projectionMatrix);
	//preBloomShader.setMat4("model", model);
	preBloomShader.setInt("diffuseTexture", 0);
	for (auto i = 0; i < lightPositions.size(); i++)
	{
		preBloomShader.setVec3("lights[" + std::to_string(i) + "].Position", lightPositions[i]);
		preBloomShader.setVec3("lights[" + std::to_string(i) + "].Color", lightColors[i]);
	}
	preBloomShader.unUse();

	lightBoxShader.use();
	lightBoxShader.setMat4("projection", projectionMatrix);
	
	lightBoxShader.unUse();
	
	/*shaderCube.use();
	shaderCube.setMat4("projection", projectionMatrix);
	shaderCube.unUse();*/
	blurShader.use();
	blurShader.setInt("image", 0);
	blurShader.unUse();
	

	int nVertex = sizeof(cubeVertices2) / (sizeof(GLfloat) * 8);
	
	// ��ʼ��Ϸ��ѭ��
	glEnable(GL_DEPTH_TEST);
	glm::vec3 targetPos = totalBoundingBox.GetCenter();

	bool bFirstIter = true;//��һ��ģ����ʶ
	while (!glfwWindowShouldClose(window))
	{

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame; // ��һ֡��ʱ��
		processInput(window, camera);
		glfwPollEvents(); // ����������� ���̵��¼�

#pragma region  ���Ƶ�hdrFBO
		glBindFramebuffer(GL_FRAMEBUFFER,hdrFBO);
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
		// �����ɫ������ ����Ϊָ����ɫ
		glClearColor(0.18f, 0.04f, 0.14f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// ������д�������ƴ���
		glBindVertexArray(VAOId);
		float distance = glm::length(targetPos - camera.Position);
		targetPos = camera.Position + distance * camera.Front;
		glm::mat4 viewMatrix = camera.GetViewMatrix(targetPos);
		preBloomShader.use();
		//����ƶ�����ͷ���򲻱�
		preBloomShader.setMat4("view", viewMatrix);
		preBloomShader.setVec3("viewPos", camera.Position);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, woodTexId);
		for (int i = 0; i < vecCubeModelMtx.size(); i++)
		{
			if (i > 0)
			{
				glBindTexture(GL_TEXTURE_2D, containTexId);
			}
			preBloomShader.setMat4("model",vecCubeModelMtx[i]);
			glDrawArrays(GL_TRIANGLES, 0, nVertex);
		}
		preBloomShader.unUse();
		//���Ƶƹ�
		lightBoxShader.use();
		lightBoxShader.setMat4("view", viewMatrix);
		for (int i = 0; i < lightPositions.size(); i++)
		{
			model = glm::mat4(1.0);
			model = glm::translate(model, lightPositions[i]);
			model = glm::scale(model, glm::vec3(0.25, 0.25, 0.25));
			
			lightBoxShader.setMat4("model", model);
			
			lightBoxShader.setVec3("lightColor", lightColors[i]);
			glDrawArrays(GL_TRIANGLES, 0, nVertex);
		}
		lightBoxShader.unUse();
#pragma endregion
#pragma region �Գ�����ֵ���Ȳ���ģ��
		 //hdrColorBuffer[1] �����ǳ�����ֵ���Ȳ���
		glBindVertexArray(quadVAO);
		bool bhorizontal = true; //��ʼ����Ϊˮƽ
		
		
		int lastId = 0;
		blurShader.use();
	
		for (int i = 0; i < amount; i++)
		{
			lastId = i % 2;
			glBindFramebuffer(GL_FRAMEBUFFER, blurFBO[i%2]);
			glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
			blurShader.setBool("horizontal", bhorizontal);
			glActiveTexture(GL_TEXTURE0);
			
			if (bFirstIter)
			{
				glBindTexture(GL_TEXTURE_2D, hdrColorBuffer[1]);
				bFirstIter = false;
			}
			else
			{
				//�������ؽ���ˮƽ�����ģ��ʱ��Ҫ����һ����ֱģ����Ľ��blurColorBuffer[1]
				//�������ؽ�����ֱ�����ģ��ʱ��Ҫ����һ��ˮƽģ����Ľ��blurColorBuffer[0]
				int textureIndex = bhorizontal ? 1 : 0;
				glBindTexture(GL_TEXTURE_2D, blurColorBuffer[textureIndex]);//index �� 0��1,0,1....��ˮƽ����ֱ��ˮƽ����ֱ...)
			}
			
			glDrawArrays(GL_TRIANGLES, 0, 6);
			bhorizontal = !bhorizontal;
		}
		blurShader.unUse();
		//blurShader.use();
		//bool horizontal = true, first_iteration = true;
		//int amount = 10;
		//
		//for (unsigned int i = 0; i < amount; i++)
		//{
		//	glBindFramebuffer(GL_FRAMEBUFFER, blurFBO[horizontal]);
		//	blurShader.setInt("horizontal", horizontal);
		//	glActiveTexture(GL_TEXTURE0);
		//	
		//	glBindTexture(GL_TEXTURE_2D, first_iteration ? hdrColorBuffer[1] : blurColorBuffer[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
		//	int nerr = glGetError();
		//	glDrawArrays(GL_TRIANGLES, 0, 6);
		//	nerr = glGetError();
		//	horizontal = !horizontal;
		//	if (first_iteration)
		//		first_iteration = false;
		//}
		//blurShader.unUse();
#pragma endregion

#pragma region ���Ƶ����ճ���
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
		BloomShader.use();
		BloomShader.setFloat("exposure", exposure);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, hdrColorBuffer[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, blurColorBuffer[!bhorizontal]);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		BloomShader.unUse();
#pragma endregion

#pragma region create element
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::Begin("Hello Bloom");
		
		ImGui::SliderFloat("EXPOSURE VALUE", &exposure, 0, 10);
		ImGui::DragInt("AMOUNT VALUE", &amount, 2,0,20);
		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#pragma endregion
		
		glBindVertexArray(0);
		glUseProgram(0);

		glfwSwapBuffers(window); // ��������
		
	}
	// �ͷ���Դ
	glDeleteVertexArrays(1, &VAOId);
	glDeleteBuffers(1, &VBOId);
#pragma region Cleanup imgui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
#pragma endregion

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
	
	if (glfwGetKey(ptrWindow, GLFW_KEY_SPACE) == GLFW_RELEASE)
	{
		hdrKeyPressed = false;
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