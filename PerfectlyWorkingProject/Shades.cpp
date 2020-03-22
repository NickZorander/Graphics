#include "Shades.hpp"
#include "objloader.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "controls.hpp"
#include <stdio.h>
#include <stdlib.h>


// The fullscreen quad's FBO
const GLfloat quadVAO[] = {
	-1.0f, -1.0f, 
	 1.0f, -1.0f, 
	-1.0f,  1.0f, 
	-1.0f,  1.0f, 
	 1.0f, -1.0f, 
	 1.0f,  1.0f, 
};

const GLfloat uvsVAO[] = {
	 0.0f,  0.0f,
	 1.0f,  0.0f,
	 0.0f,  1.0f,
	 0.0f,  1.0f,
	 1.0f,  0.0f,
	 1.0f,  1.0f,
};


ShadesObject::ShadesObject(	const char * obj_path,
							const char * scene_vertex_shader_path,
							const char * scene_fragment_shader_path,
							const char * texture_path,
							const char * result_vertex_shader_path,
							const char * result_fragment_shader_path,
							glm::vec3 ws_pos,
							glm::vec3 lp)
	
	:obj(	obj_path,
			scene_vertex_shader_path,
			scene_fragment_shader_path,
			texture_path,
			ws_pos,
			lp
		)
{
	//��� ������ �������� ������ ������ ����� � ��� �� �������� ���:
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	//����� �� �������� ���������� ������, ������� ����������� � ������������ ����� ��������� ������. 
	//����� �� ������ ������� �������� ������ �������� ���� ����������, � ��������� �� ������ ��������� ������:
	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	// ������������ �������� � ������� �������� ��������� ������
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

	//�������� ������� ������������ ����������. ����� ������� ���� � ���, ��� �� ���������� ������� ����������� ����� ������� � ���������
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, windowWidth, windowHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	//��� ������ �� ��������� ������ ��� ������� � ����� ��� ����������.
	//����� �� ������������ ������ ������������ � ����������� ����� ������������ ������� � ��������� ������ ����� :
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	//��������� �������� ����� �������� ������ ����� �� ������� � ������� ����������� ���������, ���� ��� �� ��� :
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		printf("ERROR::FRAMEBUFFER:: Framebuffer is not complete!/n");
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	
	quadProgramID = LoadShaders(result_vertex_shader_path, result_fragment_shader_path);
	
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
	
	//�������� � ����� �������
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(glm::vec3), &quadVAO[0], GL_STATIC_DRAW);

	//�������� � ����� ��������
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(glm::vec2), &uvsVAO[0], GL_STATIC_DRAW);
}

void ShadesObject::Draw()
{
	
	// ������ ������
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // ����� ��������� �� ������������
	glEnable(GL_DEPTH_TEST);
	obj.Draw();
	
	
	// ������ ������
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // ���������� ����� ����� �� ���������
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(quadProgramID);
	glBindVertexArray(VertexArrayID);
	glDisable(GL_DEPTH_TEST);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute
		2,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glVertexAttribPointer(
		1,                                // attribute
		2,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

ShadesObject::~ShadesObject()
{
}
