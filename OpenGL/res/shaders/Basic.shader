#shader vertex
#version 330 core

layout(location = 0) in vec4 position; //���� �ִ� location = 0��, ������ �ؼ��κ��� index 0�� �ǹ���

void main()
{
	gl_Position = position; //119���� ���ٽ���, 2���� ���� ����������, �˾Ƽ� vec4�� ��ȯ����
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color; //��� color

void main()
{
	color = vec4(1.0, 1.0 ,0.0, 1.0); //R 1, G 1, B 0 (�����), ���� 1 ���� ����
};