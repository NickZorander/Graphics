#include "Object.hpp"
#include "objloader.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "controls.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


my_Object::my_Object(	const char * obj_path,						//���� �������
						const char * vertex_shader_path,			//���� ���������� �������
						const char * fragment_shader_path,			//���� ������������ �������
						const char * texture_path,					//���� �������
						glm::vec3 ws_pos,							//������� � ������� ������������ 
						glm::vec3 lp 								//�������� �����
){	
	Position_worldspace = ws_pos;

	LightPos = lp;

	//VAO
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
	
	// ������� ���������
	ProgramID = LoadShaders(vertex_shader_path, fragment_shader_path);

	// Get a handle for our "MVP" uniform
	MatrixID = glGetUniformLocation(ProgramID, "MVP");
	ViewMatrixID = glGetUniformLocation(ProgramID, "V");
	ModelMatrixID = glGetUniformLocation(ProgramID, "M");

	//��������� ��������
	Texture = loadDDS(texture_path); 

	//��������� ��� ������
	loadOBJ(obj_path, vertices, uvs, normals);

	//�������� � ����� �������
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	//�������� � ����� ��������
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

	//�������� � ����� �������
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

	//�������� uniform handler
	LightID = glGetUniformLocation(ProgramID, "LightPosition_worldspace");
}




void my_Object::Draw()
{
	// ���� �������
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	
	//���������� ������ ����� �������
	glBindVertexArray(VertexArrayID);
	
	// ���������� ���� ���������
	glUseProgram(ProgramID);

	// ��������� ��� ����������� �������
	computeMatricesFromInputs();
	glm::mat4 ProjectionMatrix = getProjectionMatrix();								//������� ��������
	glm::mat4 ViewMatrix = getViewMatrix();											//������� ����
	glm::mat4 ModelMatrix = glm::translate(glm::mat4(1.0f), Position_worldspace);	//��������� �������

	glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

	// ��������� ��������
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
	glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

	
	glUniform3f(LightID, LightPos.x, LightPos.y, LightPos.z);

	// ������ ��������
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture);
	glUniform1i(TextureID, 0);

	//�������
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                 
		3,                  
		GL_FLOAT,           
		GL_FALSE,           
		0,                  
		(void*)0            
	);

	// UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glVertexAttribPointer(
		1,                                
		2,                                
		GL_FLOAT,                         
		GL_FALSE,                        
		0,                                
		(void*)0                          
	);

	// �������
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glVertexAttribPointer(
		2,                                
		3,                                
		GL_FLOAT,                         
		GL_FALSE,                         
		0,                               
		(void*)0                          
	);

	// Draw the triangles !
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	
}


my_Object::~my_Object()
{
	// ������ VBO � ������
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteBuffers(1, &normalbuffer);
	glDeleteProgram(ProgramID);
	glDeleteTextures(1, &Texture);
	glDeleteVertexArrays(1, &VertexArrayID);
}