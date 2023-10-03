#pragma once
#include "../glad/glad.h"
#include "../gameObjects/CircleObj.h"
#include "../gameObjects/PolygonObj.h"
#include "../gameObjects/Flipper.h"

class Renderer
{
public:
	Renderer();
	~Renderer();
	void StartFrame();
	void DrawCircle(const CircleObj& circle);
	void DrawPolygon(const PolygonObj& poly);
	void DrawFlipper(const Flipper& flip);

	void SetCamera(const Pos2F& pos, const Pos2F& scale);
	void FinalizeFrame();
private:
	GLuint vbo;
	GLuint vao;
	GLuint shaderProgram;
	static const int vboSize = 100000;
	float* vboContents;
	float* currentVboLoc;
	int currentTrianglesDrawn;
	static const int vertexSize = 6;
	static const int maxTriangles = vboSize / vertexSize;

	Pos2F cameraPos, cameraScale;
	GLint posAttrib, colAttrib;
	GLuint vertexShader, fragmentShader;
};

