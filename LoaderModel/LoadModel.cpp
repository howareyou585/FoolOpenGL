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
	int materialId;//����ID
};
//���ʵĶ���
struct MaterialData
{
	string diffuse; //��ɫ��ͼ
	string specular;//�߹���ͼ
	string normal; //������ͼ
};

using namespace std;
// ���̻ص�����ԭ������
void processInput(GLFWwindow* window, Camera& camera);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);// ���������

const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;
const int AMOUNT = 100;
float deltaTime = 0.0f; // ��ǰ֡����һ֡��ʱ���
float lastFrame = 0.0f; // ��һ֡��ʱ��
float lastX = WINDOW_WIDTH / 2.0f;
float lastY = WINDOW_HEIGHT / 2.0f;
bool  bFirstMove = true;
GLuint vaoId = 0;
GLuint vboId = 0;
Camera camera;



bool importFile(
				string & strModelFile,// ģ���ļ�·��
				vector<VertexData>&vecVertices,//����vecVertices�������涥��
				vector<float>& vecValue,//����vecIndices������������
				vector<unsigned int> &vecIndices,
				vector<MeshData>&vecMeshData,
				vector<MaterialData>& vecMaterialData,
				vector<string>& vecTextureFilePath
	)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(strModelFile.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	// check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
		return false;
	}

	
	auto numMaterail = scene->mNumMaterials; //ģ���в��ʵ�����
	cout << "ģ���в��ʵ�����:" << numMaterail << endl;

	aiTextureType textureType[] = { aiTextureType_DIFFUSE , aiTextureType_SPECULAR,aiTextureType_HEIGHT };
	string textureTypeStr[] = { "��ɫ","�߹�","����" };
	string defaultTexture[] = {
		"..\\resources\\textures\\white.png", //��ɫĬ��ͼƬ
		"..\\resources\\textures\\black.png", //�߹�Ĭ��ͼƬ
		"..\\resources\\textures\\normal.png"};//����Ĭ��ͼƬ
	
	for (int i = 0; i < numMaterail; i++)
	{
		auto ptrMaterail = scene->mMaterials[i];
		aiString aiTexturePath;
		string strTexturePath;
		cout << "-------------------------------------------" << endl;
		cout << "��" << i << "����ͼ����Ϣ��" << endl;
		MaterialData tempMaterialData;
		string *ptrStr = nullptr;
		int nType = sizeof(textureType) / sizeof(aiTextureType);
		ptrStr = &tempMaterialData.diffuse;
		for (auto j = 0; j < nType; j++)
		{
			aiTexturePath = "";
			ptrMaterail->GetTexture(textureType[j], 0, &aiTexturePath);
			
			if (aiTexturePath.length > 0)
			{
				cout << textureTypeStr[j] << " ��ͼ·��=>" << aiTexturePath.C_Str() << endl;
				strTexturePath = string(aiTexturePath.C_Str());
				vecTextureFilePath.emplace_back(strTexturePath);
				*ptrStr = strTexturePath; //
			}
			else
			{
				*ptrStr = defaultTexture[j];
				cout << textureTypeStr[j] << " ��ͼ·��=>" << defaultTexture[j]<< endl;
			}
			
			ptrStr++;
		}
		vecMaterialData.emplace_back(tempMaterialData);
	}
	//ȥ�أ�ȥ���ظ�����ͼ
	std::sort(vecTextureFilePath.begin(), vecTextureFilePath.end());
	auto it = std::unique(vecTextureFilePath.begin(), vecTextureFilePath.end());
	if (it != vecTextureFilePath.end())
	{
		vecTextureFilePath.erase(it, vecTextureFilePath.end());
	}
	//��Ĭ�ϵ���ͼ���뵽vecTextureFilePath
	for (int i = 0; i < 3; i++)
	{
		vecTextureFilePath.emplace_back(string(defaultTexture[i]));
	}

	auto numMeshes = scene->mNumMeshes;
	std::cout << "mesh count = " << numMeshes << endl;
	auto meshes = scene->mMeshes;

	int nvextex = 0;
	int offset = 0;
	for (int i = 0; i < numMeshes; i++)
	{
		auto ptrMesh = meshes[i];
		if (!ptrMesh)
		{
			std::cout << "��" << i << "��mesh Ϊ NULL" << endl;
		}
		
		string strName(ptrMesh->mName.C_Str());
		//��ȡ����������Ͷ�������
		auto numVertices = ptrMesh->mNumVertices;
		//auto numNormals = ptrMesh->mNormals;

		auto ptrVertices = ptrMesh->mVertices;
		auto ptrNormals = ptrMesh->mNormals;
		auto ptrTextureCoords = ptrMesh->mTextureCoords;
		auto textureCoord = ptrTextureCoords[0];

		//mesh�еĶ�����Ϣ
		std::cout << "-------------------------------------------" << endl;
		std::cout << "��" << i << "�� mesh �Ķ�������" << numVertices << endl;
		for (auto j = 0; j < numVertices; j++)
		{
			auto position = ptrVertices[j];
			auto normal = ptrNormals[j];
			auto txtcoord = textureCoord[j];
			//std::cout << "(" << position.x << "," << position.y << "," << position.z << "," << normal.x << "," << normal.y << "," << normal.z << "," << txtcoord.x << "," << txtcoord.y << "," << txtcoord.z << ")" << endl;

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

		//��ȡ���������������
		auto numFaecs = ptrMesh->mNumFaces;
		auto faces = ptrMesh->mFaces;
		std::cout << "��" << i << "��mesh ��" << numFaecs << "����" << endl;
		MeshData md;
		md.indexNum = numFaecs * 3; // mesh������������
		md.materialId = ptrMesh->mMaterialIndex;
		std::cout << "��" << i << "��mesh ����ID:" << md.materialId << endl;
		//vecLength.emplace_back(nIndex);
		for (auto j = 0; j < numFaecs; j++)
		{
			//��ȡ�������
			auto face = faces[j];
			auto numIndices = face.mNumIndices;
			auto indices = face.mIndices;
			for (auto k = 0; k < numIndices; k++)
			{
				vecIndices.emplace_back(indices[k] + nvextex); // ����Ҫ����һ��Mesh�Ķ�������
			}

		}
		md.offsetIndex = offset;
		md.vertNum = numVertices;
		
		vecMeshData.emplace_back(md);
		nvextex += numVertices;
		offset += numFaecs * 3;//��һ��mesh ����������ƫ��
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
		
		glDrawElements(GL_TRIANGLES, meshData.indexNum, GL_UNSIGNED_INT, (void*)(meshData.offsetIndex * sizeof(float)));//���һ������Ϊ������EBO�е�ƫ��
		
	//}
}
//������Ƭ��·����ȡ����ID
unsigned int getTexId(const vector<string>&vecFilePath, const string& filePath)
{
	int texId = 0;
	auto it = std::find(vecFilePath.begin(), vecFilePath.end(), filePath);
	if (it != vecFilePath.end())
	{
		texId = std::distance(vecFilePath.begin(), it);
	}
	return texId;
}

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

	GL_INPUT_ERROR
	vector<VertexData>vecVertices;//����vecVertices�������涥��
	vector<float> vecValue;//����vecIndices������������
	vector<unsigned int> vecIndices;
	vector<MeshData>vecMeshData;
	vector<MaterialData> vecMaterialData;
	vector<string> vecTextureFilePath;
	//string strPath = "../resources/models/3dmax/baseModel.obj";//baseScene.FBX
	string strPath = "../resources/models/3dmax/baseScene.FBX";
	if (!importFile(strPath, vecVertices, vecValue, vecIndices, vecMeshData,vecMaterialData,vecTextureFilePath))
	{
		return -1;
	}
	
	VAOBuffer vaoBuffer;
	vaoBuffer.BuildVAO(vecVertices, vecIndices);
	vaoId = vaoBuffer.GetVAO();
	vboId = vaoBuffer.GetVBO();
	
	map< int, GLuint >mapTexUnit2TextureId;
	Shader shader("model.vertex", "model.frag");
	//������Ԫ
	
	for (auto i = 0; i < vecTextureFilePath.size(); i++)
	{
		auto texid = TextureFromFile(vecTextureFilePath[i].c_str(), "");
		mapTexUnit2TextureId[i] = texid;
	
	}
	

	BoundingBox box;
	box.Merge(vecValue.data(), vecValue.size(), 3);
	
	camera.InitCamera(box, 0.8);
	glm::vec3 targetPos = box.GetCenter();
	// �����ӿڲ���
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	
	glEnable(GL_DEPTH_TEST);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// ��ʼ��Ϸ��ѭ��
	int angle = 0;
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame; // ��һ֡��ʱ��
		processInput(window, camera);
		glfwPollEvents(); // ����������� ���̵��¼�
		
		// �����ɫ������ ����Ϊָ����ɫ
		glClearColor(0.18f, 0.04f, 0.14f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		++angle;
		angle = angle % 360;
		shader.use();
		
		//model = glm::rotate(model, glm::radians((float)angle), glm::vec3(0.0, 1.0, 0));
		glm::mat4 view = camera.GetViewMatrix(targetPos);
		
		glm::mat4 projection = camera.GetProjectionMatrix(((float)WINDOW_WIDTH)/((float)WINDOW_HEIGHT));
		
		
		int index = 0;
		for (auto i = 0; i < vecMeshData.size(); i++)
		{
			auto& meshData = vecMeshData[i];
			auto materialId = meshData.materialId;
			
			auto texUnitId = getTexId(vecTextureFilePath, vecMaterialData[materialId].diffuse);
			if (texUnitId <0)
			{
				continue;
			}
			shader.setInt("t0", texUnitId);

			glActiveTexture(GL_TEXTURE0+ texUnitId);
			auto texId = mapTexUnit2TextureId[texUnitId];
			glBindTexture(GL_TEXTURE_2D, texId);
			index = i * (-1);
			draw(meshData, shader,
				view, projection,
				glm::vec3(index * 3, index * 3, index * 3),
				angle, glm::vec3(0.f, 1.f, 0.f), 
				glm::vec3(0.15f, 0.15f, 0.15f));
		}
		
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