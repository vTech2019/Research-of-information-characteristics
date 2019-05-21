R"(
#version 430 core

in vec3 position;
in vec3 color;
in vec2 texture;

uniform mat4 projectionMatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;

out vec3 _color;
out vec2 _texture_coord;

float function(float x, float y) {
	return 1.0f - x * x + y * y;
}
void main(){
		_texture_coord = texture;
		_color = color;
		gl_Position =  viewMatrix * modelMatrix * projectionMatrix * vec4(position, 1.0f);
}

)"