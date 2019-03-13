R"Text(

#version 430 compatibility

in vec3 _colors;

void main() {


	gl_FragColor = vec4(_colors, 1.0f);
}

)Text"