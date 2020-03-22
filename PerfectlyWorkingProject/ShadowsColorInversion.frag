#version 330 core

// Ouput data
layout(location = 0) out vec4 color;

uniform sampler2D my_texture;

in vec2 UV;

void main(){
	color =  vec4(1.0, 1.0, 1.0, 1.0) - texture(my_texture, UV);
}