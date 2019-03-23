R"TEXT(
#version 430 core

in vec3 positions;
in vec3 colors;

uniform mat4 projectionMatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;

out vec3 _colors;

float function(float x, float y) {
	return 1.0f - x * x + y * y;
}
void main(){
		_colors = colors;
		vec4 position =  viewMatrix * modelMatrix * projectionMatrix * vec4(positions.xyz , 1.0f);
		gl_Position =vec4(position.xy, position.z, position.w);

}

)TEXT"