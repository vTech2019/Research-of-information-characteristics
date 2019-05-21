R"(

#version 430 compatibility

in vec3 _color;
in vec2 _texture_coord;
uniform sampler2D texture;

void main() {
	
	gl_FragColor =  texture2D(texture, _texture_coord)/** vec4(_color, 1.0)*/; 
}

)"