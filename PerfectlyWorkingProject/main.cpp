#define GLEW_STATIC
// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <shader.hpp>
#include <texture.hpp>
#include <controls.hpp>
#include <objloader.hpp>
#include <Object.hpp>
#include <MyWindow.hpp>
#include <BM_Object.hpp>
#include <trans.hpp>
#include <Shades.hpp>
#include <ShadowMap.hpp>

//////////////////////////////////////------------------------------------------------ 1 ~ ����� �����, 0 ~ ��� ���������-----------------------------
#define SHADES_AND_POSTEFF 1
/////////////////////////////////////-----------------------------------------------------------------------------------------------------------------


// ������ ������� ���� ��� 1024�768
int windowWidth = 1024;
int windowHeight = 768;



int main(void)
{	
	//Window init
	MakeWindow(windowWidth, windowHeight);
	
	
	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	
	if (SHADES_AND_POSTEFF)
	{		
		// ������� ���� �������
		glEnable(GL_DEPTH_TEST);

		// ������ ��������, ���� �� ����� ���� � ������
		glDepthFunc(GL_LESS);

		// ����������� ������������, ��� ������� �� ������� �� ������ 
		glEnable(GL_CULL_FACE);

		GLuint VertexArrayID;
		glGenVertexArrays(1, &VertexArrayID);
		glBindVertexArray(VertexArrayID);

		// ��������� ��� �������� �������
		GLuint depthProgramID = LoadShaders("DepthRTT.vert", "DepthRTT.frag");

		
		GLuint depthMatrixID = glGetUniformLocation(depthProgramID, "depthMVP");

		// ��������� ��������
		GLuint Texture = loadDDS("uvmap.DDS");

		// ����������� ��� ������
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec2> uvs;
		std::vector<glm::vec3> normals;
		loadOBJ("room_thickwalls2.obj", vertices, uvs, normals);

		
		

		// ��������� �� VBO

		//�������� � ����� �������
		GLuint vertexbuffer;
		glGenBuffers(1, &vertexbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

		//�������� � ����� ��������
		GLuint uvbuffer;
		glGenBuffers(1, &uvbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

		//�������� � ����� �������
		GLuint normalbuffer;
		glGenBuffers(1, &normalbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);


		// ---------------------------------------------
		// ����� �������� ������ � ��������
		// ---------------------------------------------

		// ������� �����������
		GLuint FramebufferName = 0;
		glGenFramebuffers(1, &FramebufferName);
		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

		// �������� �������
		GLuint depthTexture;
		glGenTextures(1, &depthTexture);
		glBindTexture(GL_TEXTURE_2D, depthTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, windowWidth, windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

		//���������� ���� �������� � ����������� ��� �����, ���� ����� �������� �������
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);

		//��� ��������� �����, ������ �������
		glDrawBuffer(GL_NONE);

		// ���������, ��� ��� ����������� ��������
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			printf("ERROR::FRAMEBUFFER:: Framebuffer is not complete!/n");
			return false;
		}
			


		// ���������, ������� ����������� ����� ����� � ������������ ��������� 
		GLuint programID = LoadShaders("ShadowMapping.vert", "ShadowMapping.frag");

		// ����� ��������
		GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");

		GLuint MatrixID = glGetUniformLocation(programID, "MVP");
		GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
		GLuint ModelMatrixID = glGetUniformLocation(programID, "M");
		GLuint DepthBiasID = glGetUniformLocation(programID, "DepthBiasMVP");
		GLuint ShadowMapID = glGetUniformLocation(programID, "shadowMap");

		GLuint lightInvDirID = glGetUniformLocation(programID, "LightInvDirection_worldspace");


		do {
			
			glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
			
			
			// ��� ������ � ���������� ������������ ������ ����� �� ����� ������������ ��� ������: 
			//		- �������� �� ������� �������� ��������� (�� bias � �������)
			//		- ������ �������� �����     
			//glEnable(GL_CULL_FACE);
			//glCullFace(GL_BACK); // Cull back-facing triangles -> draw only front-facing triangles

			// Clear the screen
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//------------------------------------------------ Draw Depth --------------------------------------------------------------------------
			// Use our shader
			glUseProgram(depthProgramID);


			//��������� ��������� �����
			glm::vec3 lightInvDir = glm::vec3(0.5f, 2, 2);

			// ������������ MVP ������� �� ������� ��������� �����
			glm::mat4 depthProjectionMatrix = glm::ortho<float>(-10, 10, -10, 10, -10, 20);
			glm::mat4 depthViewMatrix = glm::lookAt(lightInvDir, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
			glm::mat4 depthModelMatrix = glm::mat4(1.0);
			glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;

			//��������� ���� �������������� � ������������� �� ������ ������ ������
			glUniformMatrix4fv(depthMatrixID, 1, GL_FALSE, &depthMVP[0][0]);

			// �������
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
			glVertexAttribPointer(
				0,					// ��������������� �������
				3,                  // ������
				GL_FLOAT,           // ���
				GL_FALSE,           // ��������������?
				0,                  // ��� (��� stride)
				(void*)0            // �������� � ������
			);


			// Draw the triangles !
			glDrawArrays(GL_TRIANGLES, 0, vertices.size());

			glDisableVertexAttribArray(0);

			//----------------------------------------------------------------------------------------------------------------------------------------



			//����������� ������ �� �����
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			//glEnable(GL_CULL_FACE);
			//glCullFace(GL_BACK); // Cull back-facing triangles -> draw only front-facing triangles

			// ��������� �����
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Use our shader
			glUseProgram(programID);

			// ������� MVP ������� ��� ������
			computeMatricesFromInputs();
			glm::mat4 ProjectionMatrix = getProjectionMatrix();
			glm::mat4 ViewMatrix = getViewMatrix();
			//ViewMatrix = glm::lookAt(glm::vec3(14,6,4), glm::vec3(0,1,0), glm::vec3(0,1,0));
			glm::mat4 ModelMatrix = glm::mat4(1.0);
			glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

			//������� ��������
			glm::mat4 biasMatrix(
				0.5, 0.0, 0.0, 0.0,
				0.0, 0.5, 0.0, 0.0,
				0.0, 0.0, 0.5, 0.0,
				0.5, 0.5, 0.5, 1.0
			);

			glm::mat4 depthBiasMVP = biasMatrix * depthMVP;

			// ��������� ���� �������������� � ����������� ������
			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
			glUniformMatrix4fv(DepthBiasID, 1, GL_FALSE, &depthBiasMVP[0][0]);

			glUniform3f(lightInvDirID, lightInvDir.x, lightInvDir.y, lightInvDir.z);

			// ���������� ���� ��������� � ���������� ���� 0
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, Texture);
			
			// ��������� "myTextureSampler" ������������ Texture Unit 0
			glUniform1i(TextureID, 0);

			//���������� ����������� �������� � ��������� �������
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, depthTexture);
			glUniform1i(ShadowMapID, 1);

			// �������
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

			// ����������� �������
			glfwSwapBuffers(window);
			glfwPollEvents();

		} // Check if the ESC key was pressed or the window was closed
		while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
			glfwWindowShouldClose(window) == 0);
		
	}

	else {


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
	
	
	
			GLuint					framebuffer;
			GLuint					texColorBuffer;
			GLuint					rbo;
			GLuint					quadProgramID;
			GLuint					VertexArrayID;

			GLuint					vertexbuffer;			//������ ��������� ������
			GLuint					uvbuffer;				//����� ���������� ���������



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


			quadProgramID = LoadShaders("PassThrought.vert", "ColorInversion.frag");

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


	
			my_Object suzanne("suzanne_2.obj",
				"Blinn-Phong.vert",
				"Blinn-Phong.frag",
				"uvmap.dds",
				glm::vec3(4, 3, -3),
				glm::vec3(0, 4, 0));



			my_Object platform("basic_platform.obj",
				"FogShader.vert",
				"FogShader.frag",
				"my_chess.dds",
				glm::vec3(0, 0, 0),
				glm::vec3(0, 4, 0));

			my_Object tower("tower.obj",
				"FogShader.vert",
				"FogShader.frag",
				"my_chess.dds",
				glm::vec3(0, 0, 0),
				glm::vec3(0, 4, 0));

			BM_Object Cyll(
				//"my_cylinder.obj",
				"divan.obj",
				"BumpMapping.vert",
				"BumpMapping.frag",
				"diffuse.dds",
				//"pink_tex.dds",
				"normal.bmp",
				//"divan_normal_map.bmp",
				"specular.dds",
				//glm::vec3(-3, 2, 0),
				glm::vec3(-3, 3, -4),
				glm::vec3(0, 4, 0));

			TransparentObjects billboards(
				"billboard.obj",
				"BillboardStandardShader.vert",
				"BillboardStandardShader.frag",
				"uvmap.dds",
				glm::vec3(-5, 2, 4),
				glm::vec3(0, 4, 0));

			my_Object suzanne_cook("suzanne_2.obj",
				"CookTorrance.vert",
				"CookTorrance.frag",
				"uvmap.dds",
				glm::vec3(4, 3, 3),
				glm::vec3(0, 4, 0));


			
			
			bool post_eff_flag = false;

			GLuint simpleProgramID = LoadShaders("PassThrought.vert", "PassThrought.frag");

			do {

				if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
					post_eff_flag = true;
				else
					post_eff_flag = false;
				
				glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // ����� ��������� �� ������������
				glEnable(GL_DEPTH_TEST);
				
				// ��������� �����
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


				//������������ ���� ����� � ������
				suzanne.Draw();
				suzanne_cook.Draw();

				platform.Draw();
				tower.Draw();
				Cyll.Draw();
				billboards.Draw();


				// ������ ������
				glBindFramebuffer(GL_FRAMEBUFFER, 0); // ���������� ����� ����� �� ���������
				glClearColor(0.5f, 0.4f, 0.7f, 0.0f);
				glClear(GL_COLOR_BUFFER_BIT);

				if (post_eff_flag)
					glUseProgram(quadProgramID);
				else 
					glUseProgram(simpleProgramID);
				
				
				
				glBindVertexArray(VertexArrayID);
				glDisable(GL_DEPTH_TEST);
				glBindTexture(GL_TEXTURE_2D, texColorBuffer);

				// �������
				glEnableVertexAttribArray(0);
				glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
				glVertexAttribPointer(
					0,                  
					2,                  
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

				glDrawArrays(GL_TRIANGLES, 0, 6);

				// ����������� �������
				glfwSwapBuffers(window);
				glfwPollEvents();

			} // Check if the ESC key was pressed or the window was closed
			while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
				glfwWindowShouldClose(window) == 0);
		
	}

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}




