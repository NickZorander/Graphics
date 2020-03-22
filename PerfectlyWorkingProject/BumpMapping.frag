#version 330 core


in vec2 UV;
in vec3 Position_worldspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;

in vec3 LightDirection_tangentspace;
in vec3 EyeDirection_tangentspace;

out vec3 color;


uniform sampler2D DiffuseTextureSampler;
uniform sampler2D NormalTextureSampler;
uniform sampler2D SpecularTextureSampler;
uniform mat4 V;
uniform mat4 M;
uniform mat3 MV3x3;
uniform vec3 LightPosition_worldspace;

void main(){

	// Свойства света
	vec3 LightColor = vec3(1,1,1);
	float LightPower = 40.0;
	
	// Свойства материала 
	vec3 MaterialDiffuseColor = texture( DiffuseTextureSampler, UV ).rgb;
	vec3 MaterialAmbientColor = vec3(0.1,0.1,0.1) * MaterialDiffuseColor;
	vec3 MaterialSpecularColor = texture( SpecularTextureSampler, UV ).rgb * 0.3;

	// Нормаль в тангенцальном пространстве. V текстурная координата инвертирована, тк карта нормалей в TGA (а не в DDS!)  для лучшего качества 
	vec3 TextureNormal_tangentspace = normalize(texture( NormalTextureSampler, vec2(UV.x,-UV.y) ).rgb*2.0 - 1.0);
	
	// расстояние до света 
	float distance = length( LightPosition_worldspace - Position_worldspace );

	// Нормаль вычисленного фрагмента в пространстве камеры 
	vec3 n = TextureNormal_tangentspace;
	// Направление на свет 
	vec3 l = normalize(LightDirection_tangentspace);
	// Косинус угла между нормалью и светом  
	float cosTheta = clamp( dot( n,l ), 0,1 );

	// вектор на камеру 
	vec3 E = normalize(EyeDirection_tangentspace);
	// вектор отражения 
	vec3 R = reflect(-l,n);
	// Косинус угла между вектором на какмеру и отраженным лучом
	float cosAlpha = clamp( dot( E,R ), 0,1 );
	
	color = 
		//Фоновое
		MaterialAmbientColor +
		// Диффузное 
		MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance*distance) +
		// Бликовое
		MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,5) / (distance*distance);

}