// 引入GLEW库 定义静态链接
#define GLEW_STATIC
#include <GLEW/glew.h>
// 引入GLFW库
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

// 包含着色器加载库
#include "shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "texture.h"
// 键盘回调函数原型声明
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

// 定义程序常量
const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;
typedef struct  MaterailInfo
{
	GLuint  diffuse;
	glm::vec3  specular;
	float shininess;
}Material;

typedef struct  LightInfo
{
	glm::vec3 position;
	glm::vec3 color;
}Light;

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

	// 注册窗口键盘事件回调函数
	glfwSetKeyCallback(window, key_callback);

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

	// 设置视口参数
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	// Section1 准备顶点数据
	// 指定顶点属性数据 顶点位置
	GLfloat vertices[] = {
-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,	// A
0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,	// B
0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,	    // C
0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,	    // C
-0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,	// D
-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,	// A


-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,	// E
-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0, 1.0f,    // H
0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,	// G
0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,	// G
0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,	// F
-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,	// E

-0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,	// D
-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0, 1.0f,    // H
-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,	// E
-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,	// E
-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,	// A
-0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,	// D

0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,	// F
0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,	// G
0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,	    // C
0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,	    // C
0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,	// B
0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,	// F

0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,	// G
-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0, 1.0f,    // H
-0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,	// D
-0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,	// D
0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,	// C
0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,	// G

-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,	// A
-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,	// E
0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,	// F
0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,	// F
0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,	// B
-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,	// A
	};
	
	//glm::vec3 ambient = glm::vec3(1.0f, 0.5f, 0.31f);
	Material material;
	
	material.specular = glm::vec3(0.5f, 0.5f, 0.5f);
	material.shininess = 256.f;
	Light light;
	light.position = glm::vec3(0.0f,1.5f,2.0f);
	light.color = glm::vec3(1.f, 1.f, 1.f);
	// 创建缓存对象
	GLuint VAOId, VBOId;
	// Step1: 创建并绑定VAO对象
	glGenVertexArrays(1, &VAOId);
	glBindVertexArray(VAOId);
	// Step2: 创建并绑定VBO对象
	glGenBuffers(1, &VBOId);
	glBindBuffer(GL_ARRAY_BUFFER, VBOId);
	// Step3: 分配空间 传送数据
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// Step4: 指定解析方式  并启用顶点属性
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)(sizeof(GLfloat)*3));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)(sizeof(GLfloat) * 6));
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//GLuint lightCubeVAOId;
	//glGenVertexArrays(1,&lightCubeVAOId);
	//glBindVertexArray(lightCubeVAOId);
	//glBindBuffer(GL_ARRAY_BUFFER, VBOId);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)0);
	//glEnableVertexAttribArray(0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindVertexArray(0);
	GLuint diffuseMap  = TextureHelper::load2DTexture("../resources/textures/container2.png");
	GLuint specularMap = TextureHelper::load2DTexture("../resources/textures/container_specular.png");
	// Section2 准备着色器程序
	Shader shader("light_map.vertex", "light_map.frag");
	Shader cubeShader("light_cube.vertex", "light_cube.frag");
	glEnable(GL_DEPTH_TEST); //启用深度测试
	//glDisable(GL_DEPTH_TEST); // 禁用深度测试
	// 开始游戏主循环
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 projection;
	glm::mat4 view;
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents(); // 处理例如鼠标 键盘等事件

		// 清除颜色缓冲区 重置为指定颜色
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		double value = glfwGetTime();
		/*float radius = 5.f;
		GLfloat camx = sin(value) * radius;
		GLfloat camz = cos(value)*radius;
		glm::vec3 eyePos(camx, 0.f, camz);*/

		glBindVertexArray(VAOId);

		//light color is changing .
		glm::vec3 ambientColor = light.color * glm::vec3(0.2f);
		glm::vec3 diffuseColor = light.color * glm::vec3(0.5f);
		
		glm::vec3 eyePos(0.f, 0.f, 5.0f);
		view = glm::lookAt(eyePos, glm::vec3(0.0f, 0.0f, 0.0f),glm::vec3(0.0f, 1.0f, 0.0f));
		projection = glm::perspective(45.0f, (GLfloat)WINDOW_WIDTH / WINDOW_HEIGHT, 1.0f, 1000.0f);
		// 这里填写场景绘制代码
		
		shader.use();
		
		model = glm::rotate(model, (float)(value * 0.004), glm::vec3(1.0f, 1.0f, 1.0f));
		//model = glm::rotate(model, (float)(value * 0.005), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(shader.programId, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(shader.programId, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(shader.programId, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		glUniform1i(glGetUniformLocation(shader.programId, "material.diffuse"), 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);
		glUniform1i(glGetUniformLocation(shader.programId, "material.specular"), 1);

		
		glUniform3fv(glGetUniformLocation(shader.programId, "material.specular"), 1, &material.specular[0]);
		glUniform1f(glGetUniformLocation(shader.programId, "material.shininess"), material.shininess);
		glUniform3fv(glGetUniformLocation(shader.programId, "viewPos"), 1, &eyePos[0]);

		glUniform3fv(glGetUniformLocation(shader.programId, "light.positon"), 1, &light.position[0]);
		
		glUniform3fv(glGetUniformLocation(shader.programId, "light.ambient"), 1, &ambientColor[0]);
		glUniform3fv(glGetUniformLocation(shader.programId, "light.diffuse"), 1, &diffuseColor[0]);
		glUniform3f(glGetUniformLocation(shader.programId, "light.specular"), 1.0f, 1.0f, 1.0f);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		//also draw the lamp object
		
		cubeShader.use();
		//glBindVertexArray(lightCubeVAOId);
		glm::mat4 light_model = glm::mat4(1.0f);
		
		light_model = glm::translate(light_model, light.position);
		light_model = glm::scale(light_model, glm::vec3(0.2f, 0.2f, 0.2f));
		
		
		glUniformMatrix4fv(glGetUniformLocation(cubeShader.programId, "model"), 1, GL_FALSE, glm::value_ptr(light_model));
		glUniformMatrix4fv(glGetUniformLocation(cubeShader.programId, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(cubeShader.programId, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		/*glBindVertexArray(0);
		glUseProgram(0);*/

		glfwSwapBuffers(window); // 交换缓存
	}
	// 释放资源
	glDeleteVertexArrays(1, &VAOId);
	//glDeleteVertexArrays(1, &lightCubeVAOId);
	glDeleteBuffers(1, &VBOId);
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