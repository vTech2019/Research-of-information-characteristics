#pragma once
#include "Math.h"
#include "OpenGL_device.h"
#include <cfloat>
float3* genPositions(size_t width, size_t height, GLfloat& max, GLfloat& min, GLfloat far_draw_x, GLfloat near_draw_x, GLfloat far_draw_y, GLfloat near_draw_y);
float3* genColors(float3* positions, GLfloat max, GLfloat min, size_t length);
GLuint* genIndices(uint32_t width, uint32_t height);
float2* genTexture(GLfloat start_x, GLfloat start_y, GLfloat end_x, GLfloat end_y, size_t width, size_t height, GLfloat far_draw_x, GLfloat near_draw_x, GLfloat far_draw_y, GLfloat near_draw_y, size_t& length_array);
void makeField(OpenGL_device* device, size_t width, size_t height, GLfloat far_draw_x, GLfloat near_draw_x, GLfloat far_draw_y, GLfloat near_draw_y);
