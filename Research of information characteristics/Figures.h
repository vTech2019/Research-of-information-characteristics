#pragma once
#include "Math.h"
float4* genPositions(size_t width, size_t height, GLfloat& max, GLfloat& min, GLfloat far_draw_x, GLfloat near_draw_x, GLfloat far_draw_y, GLfloat near_draw_y);
float4* genColors(float4* positions, GLfloat max, GLfloat min, size_t length);
GLuint* genIndices(uint32_t width, uint32_t height);