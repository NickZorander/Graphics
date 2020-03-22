#version 330 core


layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_modelspace;


out vec2 UV;														//координаты текстуры самого объекта		
out vec3 Position_worldspace;										//координаты в мировом пространстве
out vec3 Normal_cameraspace;										//Нормаль
out vec3 EyeDirection_cameraspace;									//Направление на наблюдателя
out vec3 LightDirection_cameraspace;								//Направление на свет 
out vec4 ShadowCoord;												//Координаты в карте теней 


uniform mat4 MVP;													// Модель-вид-проекция 
uniform mat4 V;														//Видовая 
uniform mat4 M;														//Модельная
uniform vec3 LightInvDirection_worldspace;							//Положение источника света
uniform mat4 DepthBiasMVP;											//Смещение по матрице глубины


void main(){

	//Координаты вершины в экранном пространстве(с учетом позиции камеры)
	gl_Position =  MVP * vec4(vertexPosition_modelspace,1);
	
	//Координаты для карты теней 
	ShadowCoord = DepthBiasMVP * vec4(vertexPosition_modelspace,1);
	
	//Позиция в мировом пространстве
	Position_worldspace = (M * vec4(vertexPosition_modelspace,1)).xyz;
	
	//Вектор на наблюдателя в пространствен камеры 
	EyeDirection_cameraspace = vec3(0,0,0) - ( V * M * vec4(vertexPosition_modelspace,1)).xyz;

	// Вектор на источник света в пространстве камеры
	LightDirection_cameraspace = (V*vec4(LightInvDirection_worldspace,0)).xyz;
	
	// Нормаль вершины в пространстве камеры 
	Normal_cameraspace = ( V * M * vec4(vertexNormal_modelspace,0)).xyz; // МАТРИЦА МОДЕЛИ НИЧЕГО НЕ МАСШТАБИРУЕТ И ПОЭТОМУ ВСЕ РАБОТАЕТ
	
	// UV просто прокачиваем дальше
	UV = vertexUV;
}