#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct my_Object
{
	GLuint					VertexArrayID;			// ������ VAO
	GLuint					ProgramID;				// ID ���������	
	GLuint					Texture;				//��������
	glm::vec3				LightPos;				//��������� �����
	glm::vec3				Position_worldspace;	//��������� � ������� ������������
	
	GLuint					LightID;				//uniform handle ��������� ���������
	GLuint					MatrixID;				//uniform handle MVP �������
	GLuint					ViewMatrixID;			//uniform handle ������� �������
	GLuint					ModelMatrixID;			//uniform handle ��������� �������
	GLuint					TextureID;				//uniform handle ��� �������

	GLuint					vertexbuffer;			//������ ��������� ������
	GLuint					uvbuffer;				//����� ���������� ���������
	GLuint					normalbuffer;			//������ �������� ������
	
	std::vector<glm::vec3>	vertices;				//������ � ������������ ������
	std::vector<glm::vec2>	uvs;					//������ � ����������� ������������
	std::vector<glm::vec3>	normals;				//������ � ���������

	//�����������
	my_Object(	const char * obj_path,
				const char * vertex_shader_path, 
				const char * fragment_shader_path, 
				const char * texture_path, 
				glm::vec3 ws_pos = glm::vec3(0, 0, 0), 
				glm::vec3 lp = glm::vec3(4, 4, 4));
	
	//���������� ������
	void Draw();
	
	//my_Object() {};
	
	//����������
	~my_Object();
};