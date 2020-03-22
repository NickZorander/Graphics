#pragma once

#include <Object.hpp>


struct TransparentObjects
{
	my_Object billboard1;
	my_Object billboard2;
	my_Object billboard3;

	TransparentObjects(	const char*  obj_path,
						const char*  vertex_shader_path,
						const char*  fragment_shader_path,
						const char*  texture_path,
						glm::vec3 ws_pos = glm::vec3(0, 0, 0),
						glm::vec3 lp = glm::vec3(4, 4, 4));

	void Draw();

};
