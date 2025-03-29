#shader vertex
#version 330 core

layout(location = 0) in vec4 position; //여기 있는 location = 0가, 데이터 해석부분의 index 0을 의미함

void main()
{
	gl_Position = position; //119에서 보다시피, 2개의 값만 전달했지만, 알아서 vec4로 변환해줌
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color; //출력 color

void main()
{
	color = vec4(1.0, 1.0 ,0.0, 1.0); //R 1, G 1, B 0 (노란색), 투명도 1 으로 설정
};