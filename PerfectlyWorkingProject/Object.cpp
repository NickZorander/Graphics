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


my_Object::my_Object(	const char * obj_path,						//файл объекта
						const char * vertex_shader_path,			//файл вершинного шейдера
						const char * fragment_shader_path,			//файл фрагментного шейдера
						const char * texture_path,					//файл текстур
						glm::vec3 ws_pos,							//позиция в мировом пространстве 
						glm::vec3 lp 								//источник света
){	
	Position_worldspace = ws_pos;

	LightPos = lp;

	//VAO
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
	
	// Сделали программу
	ProgramID = LoadShaders(vertex_shader_path, fragment_shader_path);

	// Get a handle for our "MVP" uniform
	MatrixID = glGetUniformLocation(ProgramID, "MVP");
	ViewMatrixID = glGetUniformLocation(ProgramID, "V");
	ModelMatrixID = glGetUniformLocation(ProgramID, "M");

	//подкачали текстуры
	Texture = loadDDS(texture_path); 

	//подкачали сам объект
	loadOBJ(obj_path, vertices, uvs, normals);

	//закачали в буфер вершины
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	//закачали в буфер текстуры
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

	//закачали в буфер нормали
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

	//получили uniform handler
	LightID = glGetUniformLocation(ProgramID, "LightPosition_worldspace");
}




void my_Object::Draw()
{
	// Тест глубины
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	
	//используем данные этого объекта
	glBindVertexArray(VertexArrayID);
	
	// Подключаем нашу программу
	glUseProgram(ProgramID);

	// Вычисляем все необходимые матрицы
	computeMatricesFromInputs();
	glm::mat4 ProjectionMatrix = getProjectionMatrix();								//матрица проекции
	glm::mat4 ViewMatrix = getViewMatrix();											//матрица вида
	glm::mat4 ModelMatrix = glm::translate(glm::mat4(1.0f), Position_worldspace);	//модельная матрица

	glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

	// Загружаем юниформы
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
	glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

	
	glUniform3f(LightID, LightPos.x, LightPos.y, LightPos.z);

	// Биндим текстуры
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture);
	glUniform1i(TextureID, 0);

	//Вершины
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

	// Нормали
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
	// Чистим VBO и шейдер
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteBuffers(1, &normalbuffer);
	glDeleteProgram(ProgramID);
	glDeleteTextures(1, &Texture);
	glDeleteVertexArrays(1, &VertexArrayID);
}