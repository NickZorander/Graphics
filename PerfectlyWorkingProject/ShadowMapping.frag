#version 330 core


in vec2 UV;
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;
in vec4 ShadowCoord;


layout(location = 0) out vec3 color;


uniform sampler2D myTextureSampler;			//Родные Текстуры объекта							
uniform vec3 LightPosition_worldspace;		//Позиция света 
uniform sampler2DShadow shadowMap;			//Карта теней 

vec2 poissonDisk[16] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);



void main(){

	// Свойства источника освещения
	vec3 LightColor = vec3(1,1,1);
	float LightPower = 1.0f;
	
	// Material properties
	vec3 MaterialDiffuseColor = texture( myTextureSampler, UV ).rgb;
	vec3 MaterialAmbientColor = vec3(0.1,0.1,0.1) * MaterialDiffuseColor;
	vec3 MaterialSpecularColor = vec3(0.3,0.3,0.3);


	// нормированная нормаль фрагмента
	vec3 n = normalize( Normal_cameraspace );
	//Направление на источник света (от фрагменат на источник)
	vec3 l = normalize( LightDirection_cameraspace );
	// Косинус угла между нормалью и вектором света
	float cosTheta = clamp( dot( n,l ), 0,1 );
	
	// Вектор на наблюдателя
	vec3 E = normalize(EyeDirection_cameraspace);
	// Вектор отражения (куда фрагмент отражает свет) 
	vec3 R = reflect(-l,n);
	// Косинус угла между векторами на наблюдателя и отражения
	float cosAlpha = clamp( dot( E,R ), 0,1 );
	
	float visibility=1.0;

	// Фиксированное смещение
	float bias = 0.005;


	// Sample the shadow map 4 times
	for (int i=0;i<15;i++){		
		// Нахождение полностью в тени съест 16*0.2 = 0.8
		// 0.2 остенется, что по сути довольно темно 
		visibility -= 0.05*(1.0-texture( shadowMap, vec3(ShadowCoord.xy + poissonDisk[i]/700.0, (ShadowCoord.z-bias)/ShadowCoord.w) ));
	}

	
	
	color = 
		// Фоновое освещение
		MaterialAmbientColor +
		// Диффузное 
		visibility * MaterialDiffuseColor * LightColor * LightPower * cosTheta+
		// бликовое
		visibility * MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,5);

}