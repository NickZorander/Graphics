#include "trans.hpp"
#include "controls.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <map>



TransparentObjects::TransparentObjects(	const char* obj_path, 
										const char* vertex_shader_path, 
										const char* fragment_shader_path, 
										const char* texture_path, 
										glm::vec3 ws_pos, 
										glm::vec3 lp):
	billboard1(obj_path, vertex_shader_path, fragment_shader_path, texture_path, ws_pos + glm::vec3(0, 0, -1), lp),
	billboard2(obj_path, vertex_shader_path, fragment_shader_path, texture_path, ws_pos, lp),
	billboard3(obj_path, vertex_shader_path, fragment_shader_path, texture_path, ws_pos + glm::vec3(0, 0, 1), lp)
{
}

void TransparentObjects::Draw()
{
	//Тест Глубины
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);


	// Включить смешивание
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//сортируем по расстоянию до центра
	float distance1 = glm::length(getCameraPosition() - billboard1.Position_worldspace);
	float distance2 = glm::length(getCameraPosition() - billboard2.Position_worldspace);
	float distance3 = glm::length(getCameraPosition() - billboard3.Position_worldspace);

	std::map<float, my_Object*> sorted;

	sorted[distance1] = &billboard1;
	sorted[distance2] = &billboard2;
	sorted[distance3] = &billboard3;

	for (std::map<float, my_Object*>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
	{
		it->second->Draw();
	}

	glDisable(GL_BLEND);
}