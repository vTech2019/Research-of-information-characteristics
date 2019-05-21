#include "Figures.h"

GLfloat function(float x, float y) {
	return 0;
}
float3* genPositions(size_t width, size_t height, GLfloat& max, GLfloat& min, GLfloat far_draw_x, GLfloat near_draw_x, GLfloat far_draw_y, GLfloat near_draw_y) {
	float3* position = (float3*)malloc(width * height * sizeof(float3));
	GLfloat start_h = near_draw_y;
	GLfloat start_w = near_draw_x;
	GLfloat end_h = far_draw_y;
	GLfloat end_w = far_draw_x;
	GLfloat step_x = (far_draw_x - near_draw_x) / width;
	GLfloat step_y = (far_draw_y - near_draw_y) / height;
	size_t index = 0;
	for (GLfloat y = 0; y < height; y++) {
		for (GLfloat x = 0; x < width; x++) {
			GLfloat f_x = near_draw_x + x * step_x;
			GLfloat f_y = near_draw_y +  y * step_y;
			GLfloat f_z = function(x, y);
			max = max < f_z ? f_z : max;
			min = min < f_z ? min : f_z;
			position[index++] = float3(f_x, f_y, f_z - 1.0f);
		}
	}
	return position;
}
float3* genColors(float3* positions, GLfloat max, GLfloat min, size_t length) {
	float3* color = (float3*)malloc(length * sizeof(float3));
	GLfloat step = 1.0f / (2 * (max - min));
	GLfloat radius = 1.0f;
	GLfloat normalize = (2.0f * 3.1416f * radius) / max;
	if (color)
		for (size_t i = 0; i < length; i++) {
			GLfloat position = (-min + positions[i].z) * step;
			position = position < 0.0f ? 0.0f : position;
			positions[i].z = position;
			color[i] = float3(position, position, position);
		}
	return color;
}
GLuint* genIndices(uint32_t width, uint32_t height) {
	if (height > 1) {
		size_t length = size_t(width) * (height)+size_t(width - 2) * (height - 2);
		GLuint * index = (GLuint*)malloc(length * sizeof(GLuint));
		size_t offset = 0;
		ptrdiff_t _y = 0, _x = 0;
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
	return NULL;
}
float2* genTexture(GLfloat start_x, GLfloat start_y, GLfloat end_x, GLfloat end_y, size_t width, size_t height, GLfloat far_draw_x, GLfloat near_draw_x, GLfloat far_draw_y, GLfloat near_draw_y) {
	GLfloat step_x = (end_x - start_x) / width;
	GLfloat step_y = (end_y - start_y) / height;
	float2 * texture_position = (float2*)malloc(width * height * sizeof(float2));
	size_t index = 0;

	for (GLfloat y = 0; y < height; y++) {
		for (GLfloat x = 0; x < width; x++) {
			GLfloat f_x = start_x + x * step_x;
			GLfloat f_y = start_y + y * step_y;
			texture_position[index++] = float2(f_x, f_y);
		}
	}
	return texture_position;
}
void makeField(OpenGL_device * device, size_t width, size_t height, GLfloat far_draw_x, GLfloat near_draw_x, GLfloat far_draw_y, GLfloat near_draw_y) {
	width *= 2;
	height *= 2;
	GLfloat max = FLT_MIN;
	GLfloat min = FLT_MAX;
	size_t size_vertex = width * height;
	size_t number_points = width * height;
	size_t number_indices = (width) * (height)+(width - 2) * (height - 2);
	float3 * positions = genPositions(width, height, max, min, far_draw_x, near_draw_x, far_draw_y, near_draw_y);
	float3 * colors = genColors(positions, max, min, number_points);
	GLuint * indices = genIndices(width, height);
	float2 * texture_coord = genTexture(0.0f, 0.0f, 1.0f, 1.0f, width, height, far_draw_x, near_draw_x, far_draw_y, near_draw_y);
	device->pushBuffer(positions, size_vertex, sizeof(float3), _GL_VECTOR_BUFFER_);
	device->pushBuffer(colors, size_vertex, sizeof(float3), _GL_COLOR_BUFFER_);
	device->pushBuffer(indices, number_indices, sizeof(GLuint), _GL_INDEX_BUFFER_);
	device->pushBuffer(texture_coord, size_vertex, sizeof(float2), _GL_TEXTURE_BUFFER_);
	device->pushDrawObjects(number_indices );
	free(colors);
	free(positions);
	free(indices);
	free(texture_coord);
}
//float4* makeBox(float4 position, GLfloat sizeQuad) {
//	//float4* position = (float4*)malloc(9 * 4 * sizeof(float4));
//	//float4* ptr_position = position;
//	GLfloat start_h = -GLfloat(sizeQuad) / 2;
//	GLfloat start_w = -GLfloat(sizeQuad) / 2;
//	GLfloat start_z = -GLfloat(sizeQuad) / 2;
//	GLfloat end_h = GLfloat(sizeQuad) / 2;
//	GLfloat end_w = GLfloat(sizeQuad) / 2;
//	GLfloat end_z = GLfloat(sizeQuad) / 2;
//	GLfloat step = sizeQuad;
//
//}