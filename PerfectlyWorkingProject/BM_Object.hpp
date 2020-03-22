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
	GLuint					VertexArrayID;			// массив VAO
	GLuint					ProgramID;				// ID программы	
	GLuint					Texture;				//текстуры
	glm::vec3				LightPos;				//Положение света
	glm::vec3				Position_worldspace;	//Положение в мировом пространстве

	GLuint					LightID;				//uniform handle источника освещения
	
	GLuint					MatrixID;				//uniform handle MVP матрицы
	GLuint					ViewMatrixID;			//uniform handle видовой матрицы
	GLuint					ModelMatrixID;			//uniform handle модельной матрицы
	GLuint					ModelView3x3MatrixID;
	GLuint					DiffuseTextureID;
	GLuint					NormalTextureID;
	GLuint					SpecularTextureID;


	GLuint					DiffuseTexture; 
	GLuint					NormalTexture; 
	GLuint					SpecularTexture; 

	GLuint					vertexbuffer;			//буффер координат вершин
	GLuint					uvbuffer;				//буфер текстурных координат
	GLuint					normalbuffer;			//буффер нормалей вершин
	GLuint					tangentbuffer;
	GLuint					bitangentbuffer;


	std::vector<glm::vec3>	vertices;				//массив с координатами вершин
	std::vector<glm::vec2>	uvs;					//массив с текстурными координатами
	std::vector<glm::vec3>	normals;				//массив с нормалями
	std::vector<glm::vec3>	tangents;
	std::vector<glm::vec3>	bitangents;

	//Конструктор
	BM_Object(const char * obj_path,
		const char * vertex_shader_path,
		const char * fragment_shader_path,
		const char * DiffuseTexture_path,
		const char * NormalTexture_path,
		const char * SpecularTexture_path,
		glm::vec3 ws_pos = glm::vec3(0, 0, 0),
		glm::vec3 lp = glm::vec3(4, 4, 4));

	//Нарисовать объект
	void Draw();

	//Деструктор
	~BM_Object();
};