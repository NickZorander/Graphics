#pragma once


#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct BM_Object
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
	GLuint					ModelView3x3MatrixID;
	GLuint					DiffuseTextureID;
	GLuint					NormalTextureID;
	GLuint					SpecularTextureID;


	GLuint					DiffuseTexture; 
	GLuint					NormalTexture; 
	GLuint					SpecularTexture; 

	GLuint					vertexbuffer;			//������ ��������� ������
	GLuint					uvbuffer;				//����� ���������� ���������
	GLuint					normalbuffer;			//������ �������� ������
	GLuint					tangentbuffer;
	GLuint					bitangentbuffer;


	std::vector<glm::vec3>	vertices;				//������ � ������������ ������
	std::vector<glm::vec2>	uvs;					//������ � ����������� ������������
	std::vector<glm::vec3>	normals;				//������ � ���������
	std::vector<glm::vec3>	tangents;
	std::vector<glm::vec3>	bitangents;

	//�����������
	BM_Object(const char * obj_path,
		const char * vertex_shader_path,
		const char * fragment_shader_path,
		const char * DiffuseTexture_path,
		const char * NormalTexture_path,
		const char * SpecularTexture_path,
		glm::vec3 ws_pos = glm::vec3(0, 0, 0),
		glm::vec3 lp = glm::vec3(4, 4, 4));

	//���������� ������
	void Draw();

	//����������
	~BM_Object();
};