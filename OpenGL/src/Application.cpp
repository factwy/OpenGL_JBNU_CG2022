//http://glew.sourceforge.net/
//����Ʈ���� GLEW ���̳ʸ� �����ٿ�ε�. �ʿ� ���̺귯�� dependencies�� ���� �� ����
//http://glew.sourceforge.net/basic.html
//�Ʒ� ���� �ڵ���� Ȯ�� 

//GLEW_STATIC Define �ʿ�
#include <GL/glew.h> //glfw���� ���� include�ؾ� ��
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>	// Basic.shader�� �б� ���� ���̺귯�� (fstream, string, sstream)
#include <string>
#include <sstream>

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragSource;
};

//--------���̴� ���� �Ľ� �Լ�--------//
static ShaderProgramSource ParseShader(const std::string& filepath)
{
	std::ifstream stream(filepath);

	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;
	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)	//vertex ���̴� ����
			{
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos)	//fragment ���̴� ����
			{
				type = ShaderType::FRAGMENT;
			}
		}
		else
		{
			ss[(int)type] << line << '\n';	// �ڵ带 stringstream�� ����
		}
	}

	return { ss[0].str(), ss[1].str() };
}

//--------Shader ������ �Լ�----------//
static unsigned int CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type); //���̴� ��ü ����(��������)
	const char* src = source.c_str();
	glShaderSource(id, // ���̴��� �ҽ� �ڵ� ���, �ҽ� �ڵ带 ����� ���̴� ��ü id
		1, // �� ���� �ҽ� �ڵ带 ����� ������
		&src, // ���� �ҽ� �ڵ尡 ����ִ� ���ڿ��� �ּҰ�
		nullptr); // �ش� ���ڿ� ��ü�� ����� ��� nullptr�Է�, �ƴ϶�� ���� ���
	glCompileShader(id); // id�� �ش��ϴ� ���̴� ������

	// Error Handling(������ ���̴� ���α׷����Ҷ� ���Ӵ�...)
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result); //���̴� ���α׷����κ��� ������ ���(log)�� ����
	if (result == GL_FALSE) //�����Ͽ� ������ ���
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length); //log�� ���̸� ����
		char* message = (char*)alloca(length * sizeof(char)); //stack�� �����Ҵ�
		glGetShaderInfoLog(id, length, &length, message); //���̸�ŭ log�� ����
		std::cout << "���̴� ������ ����! " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id); //������ ������ ��� ���̴� ����
		return 0;
	}

	return id;
}

//--------Shader ���α׷� ����, ������, ��ũ----------//
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragShader)
{
	unsigned int program = glCreateProgram(); //���̴� ���α׷� ��ü ����(int�� ����Ǵ� ���� id)
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragShader);

	//�����ϵ� ���̴� �ڵ带 program�� �߰��ϰ� ��ũ
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	//���̴� ���α׷��� ���������Ƿ� vs, fs ���� ���α׷��� ���̻� �ʿ� ����
	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	// glfwMakeContextCurrent�� ȣ��� �Ŀ� glewInit�� ����Ǿ�� ��
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Error\n";
	}

	std::cout << glGetString(GL_VERSION) << std::endl; //�� �÷����� GL_Version ����غ���

	//glEnable(GL_CULL_FACE);
	float position[] = {
		-0.5f, -0.5f, 0.0f,	//0
		0.5f, -0.5f, 0.0f,	//1
		0.5f, 0.5f, 0.0f,	//2
		-0.5f, 0.5f, 0.0f	//3
	};

	unsigned int indices[] = {	//index buffer�� �Բ� ��� (index�� unsigned int type)
		0, 1, 2,	//t1
		2, 3, 0		//t2
	};

	//--------�����͸� �����ϴ� ����--------//
	unsigned int bufferID;
	glGenBuffers(1, &bufferID);										//1. ���� ����
	glBindBuffer(GL_ARRAY_BUFFER, bufferID); // bind == activate	//2. ���ε� ("�۾� ����")
	glBufferData(GL_ARRAY_BUFFER,	// ���� CPU -> GPU				//3. �۾� ���� ���ۿ� ������ ����
					12 * sizeof(float),
					position,
					GL_STATIC_DRAW);

	unsigned int ibo;	//Index Buffer Object
	glGenBuffers(1, &ibo);												//1. �ε��� ���� ����
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);	//<-- bind  == activate	//2. ���ε� ("�۾� ����"), �������� �޸� GL_ELEMENT_ARRAY_BUFFER
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,	//���� CPU -> GPU			//3. �۾� ���� ���ۿ� ������ ����
				6 * sizeof(unsigned int),
				indices,
				GL_STATIC_DRAW);

	// �����͸� �ؼ��ϴ� ���
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,	// Index (location)
							3,	// �ϳ��� vertex�� ��� �����͸� �ѱ����(n����)
							GL_FLOAT,	// Data Type
							GL_FALSE,	// Normalization
							sizeof(float) * 3,	// stride
							0);	// offset

	// Shader
	ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");

	unsigned int shader = CreateShader(source.VertexSource, source.FragSource);	// GPU�� �����ϴ� Shader�� index
	glUseProgram(shader); //active (bind)
	//draw call�� �۾� ������ ���̴� ���α׷��� ����Ͽ� �۾� ������ ���� �����͸� �׸�

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		//glDrawArrays(GL_TRIANGLES, 0, 3); //draw call
		glDrawElements(GL_TRIANGLES,	//draw call
						6,
						GL_UNSIGNED_INT,
						nullptr);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glDeleteProgram(shader);
	glfwTerminate();
	return 0;
}