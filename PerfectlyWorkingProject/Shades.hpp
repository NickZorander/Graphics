#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Object.hpp>

extern int windowWidth;
extern int windowHeight;



struct ShadesObject
{
	my_Object				obj;
	
	GLuint					framebuffer;
	GLuint					texColorBuffer;
	GLuint					rbo;
	GLuint					quadProgramID;
	GLuint					VertexArrayID;

	GLuint					vertexbuffer;			//буффер координат вершин
	GLuint					uvbuffer;				//буфер текстурных координат



	//Конструктор
	ShadesObject(	const char * obj_path,
					const char * scene_vertex_shader_path,
					const char * scene_fragment_shader_path,
					const char * texture_path,
					const char * result_vertex_shader_path,
					const char * result_fragment_shader_path,
					glm::vec3 ws_pos = glm::vec3(0, 0, 0),
					glm::vec3 lp = glm::vec3(4, 4, 4));

	//Нарисовать объект
	void Draw();

	//my_Object() {};

	//Деструктор
	~ShadesObject();
};