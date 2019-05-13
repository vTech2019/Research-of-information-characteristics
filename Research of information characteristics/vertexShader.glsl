R"(
#version 430 core

in vec4 position;
in vec4 color;

uniform mat4 projectionMatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;

out vec4 _color;

float function(float x, float y) {
	return 1.0f - x * x + y * y;
}
void main(){
		_color = color;
		gl_Position =  viewMatrix * modelMatrix * projectionMatrix * position;
}

)"