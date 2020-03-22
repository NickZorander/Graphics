#version 330 core

in vec2 UV;							
in vec3 Position_worldspace;			
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;

layout(location = 0) out vec4 out_color;

uniform sampler2D my_texture;


const vec3 fogColor = vec3(0.5, 0.5,0.5);
const float init_FogDensity = 1.5;


float FogDensity (float y)
{
	//float y= x + 0.4;
	
	//return init_FogDensity*(-sqrt(abs(y))+1);
	return init_FogDensity*(exp(-y));
	
}

void main()
{
	//FOG
	vec3 color = texture(my_texture, UV).rgb;	
	vec3 finalColor = vec3(0, 0, 0);
	float dist = 0;
	float fogFactor = 0;
   	dist = 0.1*length(EyeDirection_cameraspace);
	//dist = 0.1 - eye_cam.y;
 	fogFactor = 1.0 / exp((dist * FogDensity(Position_worldspace.y))*(dist* FogDensity(Position_worldspace.y)));
   	fogFactor = clamp(fogFactor, 0.0, 1.0);
	//fogFactor = 0.3;
	finalColor = mix(fogColor, color, fogFactor);
	//float be = (5.0 + eye_cam.y) * 0.05;
	//float bi = (5.0 + eye_cam.y) * 0.02;
 	//float ext = exp(-dist * be);
	//float insc = exp(-dist * bi);
 
	//finalColor = color * ext + fogColor * (1 - insc);
	//out_color = vec4(finalColor, 1);
	vec3 p = normalize(Position_worldspace);
	
//	if (Position_worldspace.y < 0.6)
//			finalColor = mix(fogColor, color, fogFactor);
//	else
//	if (Position_worldspace.y < 0.7)
//			finalColor = mix(fogColor, color, fogFactor + 0.01);
//	else
//	if (Position_worldspace.y < 0.8)
//			finalColor = mix(fogColor, color, fogFactor + 0.02);
//	else
//	if (Position_worldspace.y < 0.9)
//			finalColor = mix(fogColor, color, fogFactor + 0.03);
//	else
//	if (Position_worldspace.y < 1.0)
//			finalColor = mix(fogColor, color, fogFactor + 0.04);
//	else
//	if (Position_worldspace.y < 1.1)
//			finalColor = mix(fogColor, color, fogFactor + 0.06);
//	else
//		finalColor = color;

	finalColor =  mix(fogColor, color, fogFactor);
	
	out_color = vec4(finalColor, 1);
}