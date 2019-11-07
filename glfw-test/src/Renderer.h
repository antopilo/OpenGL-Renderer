#ifndef RENDERER
#define RENDERER
#endif
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#pragma once
#include <fstream>
#include <string>
#include <sstream>


#define ASSERT(x) if (!(x)) __debugbreak();


#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

struct ShaderSource {
	std::string VertexShader;
	std::string FragmentShader;
};

void Render();

static ShaderSource ParseShader(const std::string& filePath);

unsigned int CompileShader(unsigned int type, const std::string& source);

unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);

void GLClearError();

bool GLLogCall(const char* function, const char* file, int line);

void StartRender();

void Run();

int CreateWindow();

void InitBuffer();

void EndRender();

int Close();




