// 引入GLEW库 定义静态链接
#define GLEW_STATIC
#include <GLEW/glew.h>
// 引入GLFW库
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
// 包含着色器加载库
#include "learnopengl/boundingbox.h"
#include "learnopengl/shader.h"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/Importer.hpp"
#include "learnopengl/model.h"
#include "learnopengl/filesystem.h"
#include "learnopengl/camera.h"
#include "learnopengl/vaobuffer.h"
#include "learnopengl/vertexset.h"

struct MeshData
{
	int vertNum; 
	int indexNum;
	int offsetIndex;
};
using namespace std;
// 键盘回调函数原型声明
void processInput(GLFWwindow* window, Camera& camera);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);// 定义程序常量

const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;
const int AMOUNT = 100;
float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间
float lastX = WINDOW_WIDTH / 2.0f;
float lastY = WINDOW_HEIGHT / 2.0f;
bool  bFirstMove = true;
GLuint vaoId = 0;
GLuint vboId = 0;
Camera camera;

bool importFile(
				string & strModelFile,// 模型文件路径
				vector<VertexData>&vecVertices,//定义vecVertices用来缓存顶点
				vector<float>& vecValue,//定义vecIndices用来缓存索引
				vector<unsigned int> &vecIndices,
				vector<MeshData>&vecMeshData)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(strModelFile.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	// check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
		return false;
	}

	auto numMeshes = scene->mNumMeshes;
	std::cout << "mesh count = " << numMeshes << endl;
	auto meshes = scene->mMeshes;

	//vector<int>vecOffset;//记录每个mesh顶点索引的偏移
	//vector<int>vecLength;//记录顶点索引的数量
	
	int nvextex = 0;
	int offset = 0;
	for (int i = 0; i < numMeshes; i++)
	{

		auto ptrMesh = meshes[i];
		if (!ptrMesh)
		{
			std::cout << "第" << i << "个mesh 为 NULL" << endl;
		}

		string strName(ptrMesh->mName.C_Str());
		//获取顶点的数量和顶点数组
		auto numVertices = ptrMesh->mNumVertices;
		//auto numNormals = ptrMesh->mNormals;

		auto ptrVertices = ptrMesh->mVertices;
		auto ptrNormals = ptrMesh->mNormals;
		auto ptrTextureCoords = ptrMesh->mTextureCoords;
		auto textureCoord = ptrTextureCoords[0];

		//mesh中的顶点信息
		std::cout << "-------------------------------------------" << endl;
		std::cout << "第" << i << "个 mesh 的顶点数量" << numVertices << endl;
		for (auto j = 0; j < numVertices; j++)
		{
			auto position = ptrVertices[j];
			auto normal = ptrNormals[j];
			auto txtcoord = textureCoord[j];
			std::cout << "(" << position.x << "," << position.y << "," << position.z << "," << normal.x << "," << normal.y << "," << normal.z << "," << txtcoord.x << "," << txtcoord.y << "," << txtcoord.z << ")" << endl;

			VertexData vd;
			vd.position.x = position.x;
			vd.position.y = position.y;
			vd.position.z = position.z;


			vd.normal.x = normal.x;
			vd.normal.y = normal.y;
			vd.normal.z = normal.z;

			vd.texCoord.x = txtcoord.x;
			vd.texCoord.y = txtcoord.y;
			vecVertices.emplace_back(vd);
			vecValue.push_back(position.x);
			vecValue.push_back(position.y);
			vecValue.push_back(position.z);
			//texCoord
		}

		//获取面的数量和面数组
		auto numFaecs = ptrMesh->mNumFaces;
		auto faces = ptrMesh->mFaces;
		std::cout << "第" << i << "个mesh 有" << numFaecs << "个面" << endl;
		MeshData md;
		md.indexNum = numFaecs * 3; // mesh中索引的数量
		//vecLength.emplace_back(nIndex);
		for (auto j = 0; j < numFaecs; j++)
		{
			//获取面的索引
			auto face = faces[j];
			auto numIndices = face.mNumIndices;
			auto indices = face.mIndices;
			for (auto k = 0; k < numIndices; k++)
			{
				vecIndices.emplace_back(indices[k] + nvextex); // 索引要加上一个Mesh的顶点数量
			}

		}
		md.offsetIndex = offset;
		md.vertNum = numVertices;
		vecMeshData.emplace_back(md);
		nvextex += numVertices;
		offset += numFaecs * 3;//下一个mesh 顶点索引的偏移
		//cout << "第" << i << "个mesh 有" << vecIndices.size() << "索引" << endl;

	}
	return true;
}

void draw(MeshData& meshData, Shader& shader,
	glm::mat4 &v,
	glm::mat4 &p,
	glm::vec3 translate = glm::vec3(0, 0, 0),
	double angle = 0.0,
	glm::vec3 ratation = glm::vec3(0, 1, 0),
	glm::vec3 scale = glm::vec3(1, 1, 1))
	
{
	shader.setMat4("view", v);
	shader.setMat4("projection", p);
	glBindVertexArray(vaoId);
	/*for (auto i = 0; i < vecMeshData.size(); i++)
	{*/
		glm::mat4 model(1.0f);
		model = glm::translate(model, translate);

		model = glm::scale(model, scale);
		model = glm::rotate(model, glm::radians(float(angle)), ratation);
		shader.setMat4("model", model);
		
		glDrawElements(GL_TRIANGLES, meshData.indexNum, GL_UNSIGNED_INT, (void*)(meshData.offsetIndex * sizeof(float)));//最后一个参数为索引在EBO中的偏移
		
	//}
}
int main(int argc, char** argv)
{

	if (!glfwInit())	// 初始化glfw库
	{
		std::cout << "Error::GLFW could not initialize GLFW!" << std::endl;
		return -1;
	}

	// 开启OpenGL 3.3 core profile
	std::cout << "Start OpenGL core profile version 3.3" << std::endl;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// 创建窗口
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
		"Demo of triangle", NULL, NULL);
	if (!window)
	{
		std::cout << "Error::GLFW could not create winddow!" << std::endl;
		glfwTerminate();
		return -1;
	}
	// 创建的窗口的context指定为当前context
	glfwMakeContextCurrent(window);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	//首先我们要告诉GLFW，它应该隐藏光标，并捕捉(Capture)它。
	//捕捉光标表示的是，如果焦点在你的程序上
	//（译注：即表示你正在操作这个程序，Windows中拥有焦点的程序标题栏通常是有颜色的那个，
	//而失去焦点的程序标题栏则是灰色的），光标应该停留在窗口中（除非程序失去焦点或者退出）。
	//我们可以用一个简单地配置调用来完成：
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//在调用这个函数之后，无论我们怎么去移动鼠标，光标都不会显示了，
	// 它也不会离开窗口。对于FPS摄像机系统来说非常完美。
	// 初始化GLEW 获取OpenGL函数
	glewExperimental = GL_TRUE; // 让glew获取所有拓展函数
	GLenum status = glewInit();
	if (status != GLEW_OK)
	{
		std::cout << "Error::GLEW glew version:" << glewGetString(GLEW_VERSION)
			<< " error string:" << glewGetErrorString(status) << std::endl;
		glfwTerminate();
		return -1;
	}

	GL_INPUT_ERROR
	vector<VertexData>vecVertices;//定义vecVertices用来缓存顶点
	vector<float> vecValue;//定义vecIndices用来缓存索引
	vector<unsigned int> vecIndices;
	vector<MeshData>vecMeshData;
	string strPath = "../resources/models/3dmax/baseModel.obj";
	if (!importFile(strPath, vecVertices, vecValue, vecIndices, vecMeshData))
	{
		return -1;
	}
	
	VAOBuffer vaoBuffer;
	vaoBuffer.BuildVAO(vecVertices, vecIndices);
	vaoId = vaoBuffer.GetVAO();
	vboId = vaoBuffer.GetVBO();
	
	Shader shader("model.vertex", "model.frag");

	BoundingBox box;
	box.Merge(vecValue.data(), vecValue.size(), 3);
	
	camera.InitCamera(box, 0.8);
	glm::vec3 targetPos = box.GetCenter();
	// 设置视口参数
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	
	glEnable(GL_DEPTH_TEST);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// 开始游戏主循环
	int angle = 0;
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame; // 上一帧的时间
		processInput(window, camera);
		glfwPollEvents(); // 处理例如鼠标 键盘等事件
		
		// 清除颜色缓冲区 重置为指定颜色
		glClearColor(0.18f, 0.04f, 0.14f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		++angle;
		angle = angle % 360;
		shader.use();
		
		//model = glm::rotate(model, glm::radians((float)angle), glm::vec3(0.0, 1.0, 0));
		glm::mat4 view = camera.GetViewMatrix(targetPos);
		
		glm::mat4 projection = camera.GetProjectionMatrix(((float)WINDOW_WIDTH)/((float)WINDOW_HEIGHT));
		
		/*for (auto i = 0; i < 10; i++)
		{
			glm::vec3 vec(i * 40,0,0);
			draw(vecMeshData, shader, view, projection, vec);
		}*/
		for (auto i = 0; i < vecMeshData.size(); i++)
		{
			draw(vecMeshData[i], shader, 
				view, projection,
				glm::vec3(i * 10, i * 10, i * 10),
				angle, glm::vec3(0.f, 1.f, 0.f), 
				glm::vec3(0.5f, 0.5f, 0.5f));
		}
		
		glBindVertexArray(0);
		glUseProgram(0);

		glfwSwapBuffers(window); // 交换缓存
	}
	// 释放资源
	/*glDeleteVertexArrays(1, &VAOId);
	glDeleteBuffers(1, &VBOId);*/
	glfwTerminate();
	return 0;
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE); // 关闭窗口
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
	float offsetY = lastY - ypos; // y 坐标往下越来越大。
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