#ifndef VERTEX_SET_H
#define VERTEX_SET_H
// Section1 准备顶点数据
	// 指定顶点属性数据 顶点位置
GLfloat squareVertices[] = {
	-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, //0
	1.0f, -1.0f, 0.0f,	1.0f, 0.0f, //1
	1.0f, 1.0f,  0.0f,	1.0f,1.0f,  //2
	/*-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, //0
	1.0f, 1.0f, 0.0f,   1.0f,1.0f,*/ //2
	-1.0f, 1.0f, 0.0f,  0.0f,1.0f  //3
};
GLuint squareIndexes[] = { 0,1,2,2,3,0 };
#endif
