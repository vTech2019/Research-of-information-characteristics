#pragma once
#include <Windows.h>
#include <stdint.h>
#include <stdio.h>
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

struct GLubyte3 {
	uint8_t r, g, b;
};
struct uint2 {
	uint32_t x, y;
	void operator ()(uint32_t x, uint32_t y) {
		this->x = x, this->y = y;
	}
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

struct float3 {
	GLfloat x, y,z;

	float3() {
		x = 0.0f; y = 0.0f; z = 0.0f;
	}
	float3(GLfloat _x, GLfloat _y, GLfloat _z) {
		x = _x; y = _y; z = _z;
	}
	GLfloat	operator +() {
		return x + y + z;
	}
	float3 operator +(float3 data) {
		return float3( x + data.x, y + data.y, z + data.z );
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
	GLfloat	operator [](size_t i) {
		switch (i) {
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		case 3:
			return w;
		}
		return NULL;
	}
	GLfloat	operator +() {
		return x + y + z + w;
	}
	float4 operator +(float4 data) {
		return float4{ x + data.x, y + data.y, z + data.z,w + data.w };
	}
	float4 operator *(GLfloat data) {
		return float4(x * data, y * data, z * data, w * data);
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
	bool operator ==(float4 data){
		if (data.x == x && data.y == y && data.z == z && data.w == w)
			return true;
		else
			return false;
	}
	void print() {
		printf("%f %f %f %f\n", x, y, z, w);
	}
};
struct float16 {
	GLfloat v_0, v_1, v_2, v_3, v_4, v_5, v_6, v_7, v_8, v_9, v_10, v_11, v_12, v_13, v_14, v_15;
	float16(float4 x, float4 y, float4 z, float4 w) {
		v_0 = x.x; v_1 = x.y; v_2 = x.z; v_3 = x.w; 
		v_4 = y.x; v_5 = y.y; v_6 = y.z; v_7 = y.w;
		v_8 = z.x; v_9 = z.y; v_10 = z.z; v_11 = z.w;
		v_12 = w.x; v_13 = w.y; v_14 = w.z; v_15 = w.w;
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
	void identity() {
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
	void transpose() {
		const float16 const tmp(x,y,z,w);
		x = { tmp.v_0, tmp.v_4, tmp.v_8, tmp.v_12 };
		y = { tmp.v_1, tmp.v_5, tmp.v_9, tmp.v_13 };
		z = { tmp.v_2, tmp.v_6, tmp.v_10, tmp.v_14 };
		w = { tmp.v_3, tmp.v_7, tmp.v_11, tmp.v_15 };
	}
	GLfloat* operator!() {
		return &x.x;
	}
	GLfloat	operator [](size_t i) {
		switch (i) {
		case 0:return x.x;
		case 1:	return x.y;
		case 2:	return x.z;
		case 3:	return x.w;
		case 4:return y.x;
		case 5:	return y.y;
		case 6:return y.z;
		case 7:return y.w;
		case 8:return z.x;
		case 9:return z.y;
		case 10:return z.z;
		case 11:return z.w;
		case 12:return w.x;
		case 13:return w.y;
		case 14:return w.z;
		case 15:return w.w;
		}
		return NULL;
	}
	void print() {
		printf("%f %f %f %f\n", x.x, x.y, x.z, x.w);
		printf("%f %f %f %f\n", y.x, y.y, y.z, y.w);
		printf("%f %f %f %f\n", z.x, z.y, z.z, z.w);
		printf("%f %f %f %f\n", w.x, w.y, w.z, w.w);
	}
};
float4 cross_vec3(float4 matrix_1, float4 matrix_2);
float4 normalize_vec3(float4 matrix);