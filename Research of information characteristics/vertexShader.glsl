R"(
#version 430 core

in vec4 position;
in vec4 color;
in vec2 texture;

uniform mat4 projectionMatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;

out vec4 _color;
out vec2 _texture_coord;

float function(float x, float y) {
	return 1.0f - x * x + y * y;
}
void main(){
		_texture_coord = texture;
		_color = color;
		gl_Position =  viewMatrix * modelMatrix * projectionMatrix * position;
}

)"