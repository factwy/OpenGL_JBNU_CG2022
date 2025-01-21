//http://glew.sourceforge.net/
//사이트에서 GLEW 바이너리 버전다운로드. 필요 라이브러리 dependencies에 복사 후 설정
//http://glew.sourceforge.net/basic.html
//아래 예제 코드실행 확인 

//GLEW_STATIC Define 필요
#include <GL/glew.h> //glfw보다 먼저 include해야 함
#include <GLFW/glfw3.h>
#include <iostream>

//--------Shader 컴파일 함수----------//
static unsigned int CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type); //셰이더 객체 생성(마찬가지)
	const char* src = source.c_str();
	glShaderSource(id, // 셰이더의 소스 코드 명시, 소스 코드를 명시할 셰이더 객체 id
		1, // 몇 개의 소스 코드를 명시할 것인지
		&src, // 실제 소스 코드가 들어있는 문자열의 주소값
		nullptr); // 해당 문자열 전체를 사용할 경우 nullptr입력, 아니라면 길이 명시
	glCompileShader(id); // id에 해당하는 셰이더 컴파일

	// Error Handling(없으면 셰이더 프로그래밍할때 괴롭다...)
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result); //셰이더 프로그램으로부터 컴파일 결과(log)를 얻어옴
	if (result == GL_FALSE) //컴파일에 실패한 경우
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length); //log의 길이를 얻어옴
		char* message = (char*)alloca(length * sizeof(char)); //stack에 동적할당
		glGetShaderInfoLog(id, length, &length, message); //길이만큼 log를 얻어옴
		std::cout << "셰이더 컴파일 실패! " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id); //컴파일 실패한 경우 셰이더 삭제
		return 0;
	}

	return id;
}

//--------Shader 프로그램 생성, 컴파일, 링크----------//
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragShader)
{
	unsigned int program = glCreateProgram(); //셰이더 프로그램 객체 생성(int에 저장되는 것은 id)
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragShader);

	//컴파일된 셰이더 코드를 program에 추가하고 링크
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	//셰이더 프로그램을 생성했으므로 vs, fs 개별 프로그램은 더이상 필요 없음
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

	// glfwMakeContextCurrent가 호출된 후에 glewInit이 수행되어야 함
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Error\n";
	}

	std::cout << glGetString(GL_VERSION) << std::endl; //내 플랫폼의 GL_Version 출력해보기

	float position[6] = {
		-0.5f, -0.5f,
		0.0f, 0.5f,
		0.5f, -0.5f
	};

	unsigned int bufferID;
	glGenBuffers(1, &bufferID);
	glBindBuffer(GL_ARRAY_BUFFER, bufferID); // bind == activate
	glBufferData(GL_ARRAY_BUFFER,	// 실제 CPU -> GPU
					6 * sizeof(float),
					position,
					GL_STATIC_DRAW);

	// 데이터를 해석하는 방법
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,	// Index (location)
							2,	// 하나의 vertex에 몇개의 데이터를 넘기는지(n차원)
							GL_FLOAT,	// Data Type
							GL_FALSE,	// Normalization
							sizeof(float) * 2,	// stride
							0);	// offset

	// Shader
	std::string vertexShader =
		"#version 330 core\n"
		"\n"
		"layout(location = 0) in vec4 position;" //여기 있는 location = 0가, 데이터 해석부분의 index 0을 의미함
		"\n"
		"void main()\n"
		"{\n"
		"	gl_Position = position;\n" //119에서 보다시피, 2개의 값만 전달했지만, 알아서 vec4로 변환해줌
		"}\n";

	std::string fragShader =
		"#version 330 core\n"
		"\n"
		"layout(location = 0) out vec4 color;" //출력 color
		"\n"
		"void main()\n"
		"{\n"
		"	color = vec4(1.0, 1.0 ,0.0, 1.0);\n" //R 1, G 1, B 0 (노란색), 투명도 1 으로 설정
		"}\n";

	unsigned int shader = CreateShader(vertexShader, fragShader);	// GPU에 존재하는 Shader의 index
	glUseProgram(shader); //active (bind)
	//draw call은 작업 상태인 셰이더 프로그램을 사용하여 작업 상태인 버퍼 데이터를 그림

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawArrays(GL_TRIANGLES, 0, 3); //draw call

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glDeleteProgram(shader);
	glfwTerminate();
	return 0;
}