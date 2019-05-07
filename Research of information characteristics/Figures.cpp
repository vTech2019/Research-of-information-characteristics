#include "Figures.h"

GLfloat function(float x, float y) {
	return 1.0f - x * x + y * y;
}
float4* genPositions(size_t width, size_t height, GLfloat& max, GLfloat& min, GLfloat far_draw_x, GLfloat near_draw_x, GLfloat far_draw_y, GLfloat near_draw_y) {
	float4* position = (float4*)malloc(width * height * sizeof(float4));
	float4* ptr_position = position;
	GLfloat start_h = -GLfloat(height) / 2;
	GLfloat start_w = -GLfloat(width) / 2;
	GLfloat end_h = GLfloat(height) / 2;
	GLfloat end_w = GLfloat(width) / 2;
	GLfloat step_x = (far_draw_x - near_draw_x) / width;
	GLfloat step_y = (far_draw_y - near_draw_y) / height;

	for (GLfloat y = start_h; y < end_h; y++) {
		for (GLfloat x = start_w; x < end_w; x++) {
			GLfloat f_x = x * step_x;
			GLfloat f_y = y * step_y;
			GLfloat f_z = function(x, y);
			max = max < f_z ? f_z : max;
			min = min < f_z ? min : f_z;
			*ptr_position++ = float4(f_x, f_y, f_z - 1.0f, 1.0f);
		}
	}
	return position;
}
float4* genColors(float4* positions, GLfloat max, GLfloat min, size_t length) {
	float4* color = (float4*)malloc(length * sizeof(float4));
	GLfloat step = 1.0f / (2 * (max - min));
	GLfloat radius = 1.0f;
	GLfloat normalize = (2.0f * 3.1416f * radius) / max;
	for (GLint i = 0; i < length; i++) {
		GLfloat position = (-min + positions[i].z) * step;
		position = position < 0.0f ? 0.0f : position;
		positions[i].z = position;
		color[i] = { position, position, position, 1.0f };
	}
	return color;
}
GLuint* genIndices(uint32_t width, uint32_t height) {
	int length = (width) * (height)+(width - 2) * (height - 2);
	GLuint * index = (GLuint*)malloc(length * sizeof(GLuint));
	int offset = 0;
	int _y = 0, _x = 0;
	for (uint32_t i = 0; i < height - 1; i++)
	{
		if (i % 2 == 0)
		{
			while (_x < width - 1)
			{
				index[offset++] = width * _y++ + _x;
				index[offset++] = width * _y-- + _x++;
			}
			if (i < height - 2) {
				index[offset++] = width * _y + _x;
				index[offset++] = width * (++_y + 1) + _x--;
			}
			else {
				index[offset++] = width * _y++ + _x;
				index[offset++] = width * _y + _x--;
			}
		}
		else {
			while (_x != 0)
			{
				index[offset++] = width * _y++ + _x;
				index[offset++] = width * _y-- + _x--;
			}
			if (i < height - 2) {
				index[offset++] = width * _y + _x;
				index[offset++] = width * (++_y + 1) + _x++;
			}
			else {
				index[offset++] = width * _y++ + _x;
				index[offset++] = width * _y + _x++;
			}
		}
	}
	return index;
}


float4* makeSkyBox(float4 position, GLfloat sizeQuad) {
	//float4* position = (float4*)malloc(9 * 4 * sizeof(float4));
	//float4* ptr_position = position;
	//GLfloat start_h = -GLfloat(height) / 2;
	//GLfloat start_w = -GLfloat(width) / 2;
	//GLfloat end_h = GLfloat(height) / 2;
	//GLfloat end_w = GLfloat(width) / 2;


}