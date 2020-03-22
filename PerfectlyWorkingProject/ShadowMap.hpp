#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


struct  ShadowMap
{
	GLuint VertexArrayID;
	GLuint depthProgramID;
	GLuint depthMatrixID;
	GLuint Texture;

	glm::vec3				lightInvDir;				//Положение света
	glm::vec3				Position_worldspace;	//Положение в мировом пространстве

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;

	GLuint vertexbuffer;
	GLuint uvbuffer;
	GLuint normalbuffer;

	GLuint FramebufferName;
	GLuint depthTexture;
	GLuint quad_vertexbuffer;
	GLuint quad_programID;
	GLuint texID;

	GLuint programID;

	GLuint TextureID;
	GLuint MatrixID;
	GLuint ViewMatrixID;
	GLuint ModelMatrixID;
	GLuint DepthBiasID;
	GLuint ShadowMapID;
	GLuint lightInvDirID;

	ShadowMap(
					const char * obj_path,
					const char * depth_vertex_shader_path,
					const char * depth_fragment_shader_path,
					const char * ShadowMapping_vertex_shader_path,
					const char * ShadowMapping_fragment_shader_path,
					const char * texture_path,
					const char * passthrought_vertex_shader_path,
					const char * passthrought_fragment_shader_path,
					glm::vec3 ws_pos,
					glm::vec3 lp
				);

	void Draw();

	~ShadowMap();
};