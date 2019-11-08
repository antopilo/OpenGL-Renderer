#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"

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
unsigned int shader;
float r = 0.0f;

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
	
}

void Run()
{
	// GenBuffers returns throught the second param.
	// we create the value and pass the pointer.
	// genbuffer will rewrite that return value using the pntr.

	unsigned int vao;
	GLCall(glGenVertexArrays(1, &vao));
	GLCall(glBindVertexArray(vao));

	VertexArray va;
	VertexBuffer vb(positions, 4 * 2 * sizeof(float));
	VertexBufferLayout layout;
	layout.Push<float>(2);
	va.AddBuffer(vb, layout);

	IndexBuffer ib(indices, 6);

	ShaderSource shaderSource = ParseShader("res/shaders/Basic.shader");
	shader = CreateShader(shaderSource.VertexShader, shaderSource.FragmentShader);
	glUseProgram(shader);

	addr = glGetUniformLocation(shader, "u_Color");
	ASSERT(addr != -1);

	GLCall(glBindVertexArray(0));

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		GLCall(glClear(GL_COLOR_BUFFER_BIT));

		r += 0.0001f;

		if (r >= 1.0f)
			r = 0.0f;


		glUniform4f(addr, r, 0.0f, 1.0f, 1.0f);

		va.Bind();
		ib.Bind();

		GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

		EndRender();
	}
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

