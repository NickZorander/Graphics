#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;

// Ouput data
out vec3 color;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;
uniform mat4 MV;
uniform vec3 LightPosition_worldspace;

void main(){

	// Характеристики света
	vec3 LightColor = vec3(1,1,1);
	float LightPower = 50.0f;
	
	//Свойства материала
	vec3 MaterialDiffuseColor = texture( myTextureSampler, UV ).rgb;
	vec3 MaterialAmbientColor = vec3(0.2,0.2,0.2) * MaterialDiffuseColor;
	vec3 MaterialSpecularColor = vec3(0.3,0.3,0.3);

	// расстояние до света
	float distance = length( LightPosition_worldspace - Position_worldspace );

	// нормаль 
	vec3 n = normalize( Normal_cameraspace );
	// направление на свет 
	vec3 l = normalize( LightDirection_cameraspace );
	//косинус 
	float cosTheta = clamp( dot( n,l ), 0,1 );
	
	// вектор на камеру 
	vec3 E = normalize(EyeDirection_cameraspace);
	// Направление отражения света 
	vec3 R = reflect(-l,n);
	//косинус 
	float cosAlpha = clamp( dot( E,R ), 0,1 );
	
	color = 
		// Фон
		MaterialAmbientColor +
		// Диффузная
		MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance*distance) +
		// Бликовая
		MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,5) / (distance*distance);

}