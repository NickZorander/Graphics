#version 330 core


in vec2 UV;
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;


out vec4 color;


uniform sampler2D myTextureSampler;
uniform mat4 MV;
uniform vec3 LightPosition_worldspace;

void main(){

	// Свойства света
	vec3 LightColor = vec3(1,1,1);
	float LightPower = 50.0f;
	
	//Свойства материала
	vec3 MaterialDiffuseColor = texture( myTextureSampler, UV ).rgb;
	vec3 MaterialAmbientColor = vec3(0.1,0.1,0.1) * MaterialDiffuseColor;
	vec3 MaterialSpecularColor = vec3(0.3,0.3,0.3);

	// Расстояние до света
	float distance = length( LightPosition_worldspace - Position_worldspace );

	// Нормаль 
	vec3 n = normalize( Normal_cameraspace );
	// Направление на свет 
	vec3 l = normalize( LightDirection_cameraspace );
	// Косинус
	float cosTheta = clamp( dot( n,l ), 0,1 );
	
	// Вектор на наблюдателя 
	vec3 E = normalize(EyeDirection_cameraspace);
	// Направление куда фрагмент отражает свет 
	vec3 R = reflect(-l,n);
	// Косинус
	float cosAlpha = clamp( dot( E,R ), 0,1 );
	
	color.rgb = 
		// Фон 
		MaterialAmbientColor +
		// Диффузная
		MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance*distance) +
		// Бликовая 
		MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,5) / (distance*distance);

	color.a = 0.3;
}