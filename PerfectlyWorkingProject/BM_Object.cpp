#include <BM_Object.hpp>
#include <tangentspace.hpp>
#include <shader.hpp>
#include <texture.hpp>
#include <objloader.hpp>
#include <controls.hpp>

BM_Object::BM_Object(	const char * obj_path, 
						const char * vertex_shader_path, 
						const char * fragment_shader_path, 
						const char * DiffuseTexture_path, 
						const char * NormalTexture_path, 
						const char * SpecularTexture_path, 
						glm::vec3 ws_pos,
						glm::vec3 lp
){	
	Position_worldspace = ws_pos;

	LightPos = lp;
	
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	ProgramID = LoadShaders(vertex_shader_path, fragment_shader_path);

	MatrixID = glGetUniformLocation(ProgramID, "MVP");
	ViewMatrixID = glGetUniformLocation(ProgramID, "V");
	ModelMatrixID = glGetUniformLocation(ProgramID, "M");
	ModelView3x3MatrixID = glGetUniformLocation(ProgramID, "MV3x3");

	DiffuseTexture = loadDDS(DiffuseTexture_path);
	NormalTexture = loadBMP_custom(NormalTexture_path);
	SpecularTexture = loadDDS(SpecularTexture_path);

	DiffuseTextureID = glGetUniformLocation(ProgramID, "DiffuseTextureSampler");
	NormalTextureID = glGetUniformLocation(ProgramID, "NormalTextureSampler");
	SpecularTextureID = glGetUniformLocation(ProgramID, "SpecularTextureSampler");

	loadOBJ(obj_path, vertices, uvs, normals);

	//Рассчитать базис
	computeTangentBasis(
		vertices, uvs, normals, // input
		tangents, bitangents    // output
	);

	
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

	//тангенцаль
	glGenBuffers(1, &tangentbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, tangentbuffer);
	glBufferData(GL_ARRAY_BUFFER, tangents.size() * sizeof(glm::vec3), &tangents[0], GL_STATIC_DRAW);

	//битангенцаль
	glGenBuffers(1, &bitangentbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, bitangentbuffer);
	glBufferData(GL_ARRAY_BUFFER, bitangents.size() * sizeof(glm::vec3), &bitangents[0], GL_STATIC_DRAW);

	LightID = glGetUniformLocation(ProgramID, "LightPosition_worldspace");
}




void BM_Object::Draw()
{
	//Тест глубины
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//берем нашу программу
	glUseProgram(ProgramID);

	// Рассчитываем все необходимые матрицы
	computeMatricesFromInputs();
	glm::mat4 ProjectionMatrix = getProjectionMatrix();
	glm::mat4 ViewMatrix = getViewMatrix();
	glm::mat4 ModelMatrix = glm::translate(glm::mat4(1.0f), Position_worldspace);
	glm::mat4 ModelViewMatrix = ViewMatrix * ModelMatrix;
	glm::mat3 ModelView3x3Matrix = glm::mat3(ModelViewMatrix);
	glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

	// Отправляем юниформы
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
	glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
	glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
	glUniformMatrix3fv(ModelView3x3MatrixID, 1, GL_FALSE, &ModelView3x3Matrix[0][0]);


	glUniform3f(LightID, LightPos.x, LightPos.y, LightPos.z);

	// берем диффузную текстуру
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, DiffuseTexture);
	// прикрепляем к текстурному юниту 0
	glUniform1i(DiffuseTextureID, 0);

	// Текстура нормалей
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, NormalTexture);
	// аналогично в текстурный юнит 1 
	glUniform1i(NormalTextureID, 1);

	// Бликовая текстура
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, SpecularTexture);
	// в текстурный юнит 2
	glUniform1i(SpecularTextureID, 2);

	
	// Вершины
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

	//UVs
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

	// Тангенцали
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, tangentbuffer);
	glVertexAttribPointer(
		3,                                
		3,                                
		GL_FLOAT,                         
		GL_FALSE,                         
		0,                                
		(void*)0                          
	);

	// Битангенцали
	glEnableVertexAttribArray(4);
	glBindBuffer(GL_ARRAY_BUFFER, bitangentbuffer);
	glVertexAttribPointer(
		4,                                
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
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(4);

	
}



BM_Object::~BM_Object()
{
	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteBuffers(1, &normalbuffer);
	glDeleteBuffers(1, &tangentbuffer);
	glDeleteBuffers(1, &bitangentbuffer);
	glDeleteProgram(ProgramID);
	glDeleteTextures(1, &DiffuseTexture);
	glDeleteTextures(1, &NormalTexture);
	glDeleteTextures(1, &SpecularTexture);
	glDeleteVertexArrays(1, &VertexArrayID);
}
