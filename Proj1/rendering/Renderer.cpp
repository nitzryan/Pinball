#include "Renderer.h"
#include <stdio.h>


void loadShader(GLuint shaderID, const GLchar* shaderSource) {
	glShaderSource(shaderID, 1, &shaderSource, NULL);
	glCompileShader(shaderID);
	//Let’s double check the shader compiled
	GLint status;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &status); //Check for errors
	if (!status) {
		char buffer[512]; glGetShaderInfoLog(shaderID, 512, NULL, buffer);
		printf("Shader Compile Failed. Info:\n\n%s\n", buffer);
	}
}


const GLchar* vertexSource =
"#version 150 core\n"
"in vec2 position;"
"in vec4 inColor;"
"out vec4 Color;"
"uniform vec2 CameraPos;"
"uniform vec2 CameraScale;"
"void main() {"
" vec2 centerPos = (position - CameraPos);"
" Color = inColor;"
" gl_Position = vec4(centerPos.x * CameraScale.x, centerPos.y * CameraScale.y, 0.0, 1.0);"
"}";

const GLchar* fragmentSource =
"#version 150 core\n"
"in vec4 Color;"
"out vec4 outColor;"
"void main() {"
" outColor = vec4(Color);" //(Red, Green, Blue, Alpha)
"}";


Renderer::Renderer() : cameraPos(0, 0), cameraScale(1,1)
{
	// Create shader, temporary, should move to seperate class
	// Load and COmpile Shaders
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	loadShader(vertexShader, vertexSource);
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	loadShader(fragmentShader, fragmentSource);
	//Join the vertex and fragment shaders together into one program
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glBindFragDataLocation(shaderProgram, 0, "outColor"); // set output
	glLinkProgram(shaderProgram); //run the linker

	// Create VBO
	vboContents = new float[vboSize];
	for (int i = 0; i < vboSize; i++)
		vboContents[i] = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vboSize * sizeof(float), vboContents, GL_STATIC_DRAW);

	// Create VAO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	posAttrib = glGetAttribLocation(shaderProgram, "position");
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, vertexSize * sizeof(float), 0);
	//(above params: Attribute, vals/attrib., type, isNormalized, stride, offset)
	glEnableVertexAttribArray(posAttrib); //Mark the attribute’s location as valid
	colAttrib = glGetAttribLocation(shaderProgram, "inColor");
	glVertexAttribPointer(colAttrib, 4, GL_FLOAT, GL_FALSE,
		vertexSize * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(colAttrib);
	glBindVertexArray(0); //Unbind the VAO so we don’t accidentally modify it

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Other Setup
	currentVboLoc = vboContents;
	currentTrianglesDrawn = 0;
}


Renderer::~Renderer()
{
	delete vboContents;
}

void Renderer::StartFrame()
{
	currentVboLoc = vboContents;
	currentTrianglesDrawn = 0;
}

void Renderer::DrawCircle(const CircleObj& circle)
{
	int numTriangles = circle.NumTriangles();
	if (currentTrianglesDrawn + numTriangles > maxTriangles)
	{
		printf("VBO Contents Exceeded\n");
		return;
	}
		
	circle.Render(currentVboLoc);
	currentVboLoc += (numTriangles * (vertexSize * 3));
	currentTrianglesDrawn += numTriangles;
}

void Renderer::DrawPolygon(const PolygonObj& poly)
{
	int numTriangles = poly.NumTriangles();
	if (currentTrianglesDrawn + numTriangles > maxTriangles)
	{
		printf("VBO Contents Exceeded\n");
		return;
	}

	poly.Render(currentVboLoc);
	currentVboLoc += (numTriangles * (vertexSize * 3));
	currentTrianglesDrawn += numTriangles;
}

void Renderer::DrawFlipper(const Flipper& flip)
{
	int numTriangles = flip.NumTriangles();
	if (currentTrianglesDrawn + numTriangles > maxTriangles)
	{
		printf("VBO Contents Exceeded\n");
		return;
	}

	flip.RenderFlipper(currentVboLoc);
	currentVboLoc += (numTriangles * (vertexSize * 3));
	currentTrianglesDrawn += numTriangles;
}

void Renderer::SetCamera(const Pos2F& pos, const Pos2F& scale)
{
	cameraPos = pos;
	cameraScale = scale;
}

void Renderer::FinalizeFrame()
{
	glBufferData(GL_ARRAY_BUFFER, vboSize * sizeof(float), vboContents, GL_STATIC_DRAW);

	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(shaderProgram);
	int cameraLocation = glGetUniformLocation(shaderProgram, "CameraPos");
	glUniform2f(cameraLocation, cameraPos.x, cameraPos.y);
	int cameraScaleLoc = glGetUniformLocation(shaderProgram, "CameraScale");
	glUniform2f(cameraScaleLoc, cameraScale.x, cameraScale.y);
	glBindVertexArray(vao);

	glDrawArrays(GL_TRIANGLES, 0, currentTrianglesDrawn * 3);
}
