#pragma once
#include <Windows.h>
#include <stdint.h>
#define _USE_MATH_DEFINES
#include <math.h>
#define GLEW_STATIC
#include "glew-2.1.0/include/GL/glew.h"
#ifndef _WIN64
#pragma comment(lib, "glew-2.1.0/lib/Release/Win32/glew32s.lib")
#pragma comment(lib, "opengl32.lib")
#else
#pragma comment(lib, "glew-2.1.0/lib/Release/x64/glew32s.lib")
#pragma comment(lib, "opengl32.lib")
#endif


struct GLubyte4 {
	uint8_t r, g, b, a;
};

struct uint2 {
	uint32_t x, y;
	void operator =(uint2 data) {
		x = data.x, y = data.y;
	}
	uint2 operator -(uint2 data) {
		return uint2{ x - data.x, y - data.y };
	}
};

struct int2 {
	int32_t x, y;
	void operator =(int2 data) {
		x = data.x, y = data.y;
	}
	int2 operator -(int2 data) {
		return int2{ x - data.x, y - data.y };
	}
};

struct float2 {
	GLfloat x, y;

	float2() {
		x = 0.0f; y = 0.0f;
	}
	float2(GLfloat _x, GLfloat _y) {
		x = _x; y = _y;
	}
	GLfloat	operator +() {
		return x + y;
	}
	float2 operator +(float2 data) {
		return float2{ x + data.x, y + data.y };
	}
};

struct float4 {
	GLfloat x, y, z, w;
	float4() {
		x = 0.0f; y = 0.0f; z = 0.0f; w = 0.0f;
	}
	float4(GLfloat _x, GLfloat _y, GLfloat _z, GLfloat _w) {
		x = _x; y = _y; z = _z; w = _w;
	}
	GLfloat	operator +() {
		return x + y + z + w;
	}
	float4 operator +(float4 data) {
		return float4{ x + data.x, y + data.y, z + data.z,w + data.w };
	}
	float4 operator *(GLfloat data) {
		return float4( x * data, y * data, z * data,w * data );
	}
	float4 operator *(float4 data) {
		return float4{ x * data.x, y * data.y, z * data.z,w * data.w };
	}
	float4 operator -(float4 data) {
		return float4{ x - data.x, y - data.y, z - data.z,w - data.w };
	}
	float4 operator =(float4 data) {
		x = data.x; y = data.y; z = data.z; w = data.w;
		return data;
	}
};

struct mat4x4 {
	float4 x, y, z, w;
	mat4x4() {
		x = { 1.0f, 0.0f, 0.0f, 0.0f };
		y = { 0.0f,1.0f,0.0f,0.0f };
		z = { 0.0f,0.0f,1.0f,0.0f };
		w = { 0.0f,0.0f,0.0f,1.0f };
	}
	mat4x4(float4 _x, float4 _y, float4 _z, float4 _w) {
		x = _x, y = _y, z = _z, w = _w;
	}
	mat4x4 operator *(mat4x4 data) {
		float4 _x = float4{ x.x, y.x, z.x, w.x };
		float4 _y = float4{ x.y, y.y, z.y, w.y };
		float4 _z = float4{ x.z, y.z, z.z, w.z };
		float4 _w = float4{ x.w, y.w, z.w, w.w };
		float4 x_x = data.x * _x;
		float4 x_y = data.x * _y;
		float4 x_z = data.x * _z;
		float4 x_w = data.x * _w;
		float4 y_x = data.y * _x;
		float4 y_y = data.y * _y;
		float4 y_z = data.y * _z;
		float4 y_w = data.y * _w;
		float4 z_x = data.z * _x;
		float4 z_y = data.z * _y;
		float4 z_z = data.z * _z;
		float4 z_w = data.z * _w;
		float4 w_x = data.w * _x;
		float4 w_y = data.w * _y;
		float4 w_z = data.w * _z;
		float4 w_w = data.w * _w;
		_x = { (+x_x), (+x_y), (+x_z), (+x_w) };
		_y = { (+y_x), (+y_y), (+y_z), (+y_w) };
		_z = { (+z_x), (+z_y), (+z_z), (+z_w) };
		_w = { (+w_x), (+w_y), (+w_z), (+w_w) };
		return mat4x4(_x, _y, _z, _w);;
	}
	mat4x4 operator =(mat4x4 data) {
		return mat4x4(x = data.x, y = data.y, z = data.z, w = data.w);
	}
};
float4 cross_vec3(float4 matrix_1, float4 matrix_2);
float4 normalize_vec3(float4 matrix);