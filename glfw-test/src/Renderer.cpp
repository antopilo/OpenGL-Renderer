#include "Renderer.h"


float positions[] = {
		-0.5f, -0.5f, // 0
		0.5f, -0.5f,  // 1
		0.5f, 0.5f,   // 2
		-0.5f, 0.5f   // 3
};

unsigned int indices[] = {
	0, 1, 2,
	2, 3, 0
};


GLFWwindow* window;
int addr;
float r = 0.0f;
unsigned int shader;


static ShaderSource ParseShader(const std::string & filePath)
{
	std::ifstream stream(filePath);

	enum class ShaderType {
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	ShaderType type = ShaderType::NONE;

	std::string line;
	std::stringstream ss[2];

	while (getline(stream, line))
	{
		// If the current lines contains #shader
		if (line.find("#shader") != std::string::npos)
		{
			// If the current line containes vertex
			if (line.find("vertex") != std::string::npos) {
				// set mode to vertex
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos) {
				// set mode to fragment
				type = ShaderType::FRAGMENT;
			}
		}
		else
		{
			ss[(int)type] << line << "\n";
		}
	}

	return { ss[0].str(), ss[1].str() };
}
unsigned int CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);

	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	//TODO: Error handling
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

		// We allocate the size of the array using 
		// m alloc. casted as a pointer.
		char* message = (char*)malloc(length * sizeof(char));

		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile " <<
			(type == GL_VERTEX_SHADER ? "vertex" : "Fragment") << " shader!" << std::endl;
		std::cout << message << std::endl;

		// Delete invalid shader
		glDeleteShader(id);
		return 0;

	}

	return id;
}


unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}


void GLClearError()
{
	// Loop until open gl returns no error.
	while (glGetError() != GL_NO_ERROR)
	{

	}
}

bool GLLogCall(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError())
	{
		std::cout << "[OpenGL Error] (" << error << ") at: " << std::endl <<
			function << " " << file << ":" << line << std::endl;
		return false;
	}
	return true;
}

void StartRender()
{
	/* Render here */
	GLCall(glClear(GL_COLOR_BUFFER_BIT));

}

void Render()
{
	r += 0.001f;

	if (r >= 1.0f)
		r = 0.0f;

	glUniform4f(addr, r, 0.0f, 1.0f, 1.0f);

	GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

}

int CreateWindow()
{

	

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(1280, 720, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}


	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	std::cout << glGetString(GL_VERSION) << std::endl;

	

	// Glew init. after creating a valid context.
	if (glewInit() != GLEW_OK) {
		std::cout << "GLEW initialization failed!";
	}
}



void InitBuffer()
{
	// GenBuffers returns throught the second param.
	// we create the value and pass the pointer.
	// genbuffer will rewrite that return value using the pntr.
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), &positions, GL_STATIC_DRAW);

	GLCall(glEnableVertexAttribArray(0));
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

	unsigned int ibo;
	GLCall(glGenBuffers(1, &ibo));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), &indices, GL_STATIC_DRAW));


	ShaderSource shaderSource = ParseShader("res/shaders/Basic.shader");
	std::cout << "VERTEX" << std::endl;
	std::cout << shaderSource.VertexShader << std::endl;
	std::cout << "FRAGMENT" << std::endl;
	std::cout << shaderSource.FragmentShader << std::endl;

	shader = CreateShader(shaderSource.VertexShader, shaderSource.FragmentShader);
	glUseProgram(shader);

	addr = glGetUniformLocation(shader, "u_Color");
	ASSERT(addr != -1);

	
}


void EndRender()
{

	/* Swap front and back buffers */
	GLCall(glfwSwapBuffers(window));

	/* Poll for and process events */
	GLCall(glfwPollEvents());
}


int Close()
{
	GLCall(glDeleteProgram(shader));

	glfwTerminate();
	return 0;
}

void Run()
{
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		StartRender();

		Render();

		EndRender();
	}
}