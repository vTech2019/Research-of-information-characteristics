R"TEXT(
#version 430 compatibility

in vec3 positions;
in vec3 colors;

uniform mat4 windowMatrix;

out vec3 _colors;

void main(){
		_colors = colors;
		gl_Position =    vec4(positions, 1.0f);
}

)TEXT"