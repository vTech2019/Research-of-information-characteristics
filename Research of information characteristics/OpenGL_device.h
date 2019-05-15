#pragma once
#include "Math.h"
#include <stdio.h>
#include <string>
#include <vector>
#pragma intrinsic(sin, cos)
enum { _GL_VECTOR_BUFFER_, _GL_COLOR_BUFFER_, _GL_INDEX_BUFFER_, _GL_TEXTURE_BUFFER_ };
class OpenGL_device
{
	mat4x4 projectionMatrix;
	mat4x4 viewMatrix;
	mat4x4 modelMatrix;

	GLuint last_program;
	std::vector<GLuint> programs;

	std::vector<GLuint> vertexShaders;
	std::vector<GLuint> fragmentShaders;
	std::vector<GLuint> computeShaders;

	std::vector<GLuint> indicesShaders;

	std::vector<GLint> projection_matrix_index;
	std::vector<GLint> model_matrix_index;
	std::vector<GLint> view_matrix_index;
	std::vector<GLint> vertex_index;
	std::vector<GLint> color_index;
	std::vector<GLint> normal_index;
	std::vector<GLint> texture_index;
	float4 Up = { 0.0f, 1.0f, 0.0f, 0.0f };
	float4 Center = { 0.0f, 0.0f, 0.0f, 0.0f };
	float4 Eye = { 0.0f, 0.0f, 1.0f, 0.0f };

	uint32_t mouse_wheel;

	HGLRC gl_context;
public:
	int2 new_mouse_position;
	int2 old_mouse_position;
	GLfloat y_angle = -90.0f;
	GLfloat x_angle = 0.0f;
	GLulong delta_time = 0;
	GLulong last_frame = 0;
	std::vector<size_t> number_objects;
	std::vector<GLuint> vector_buffer;
	std::vector<GLuint> color_buffer;
	std::vector<GLuint> index_buffer;
	std::vector<GLuint> texture_buffer;
	std::vector<uint2> texture_id;

	void rotate(GLfloat angle, float4 vector);
	void lookAt(float4 Eye, float4 Up, float4 Center);
	void genProjection(GLfloat width, GLfloat height, GLfloat z_near, GLfloat z_far, GLfloat FOV);
	void checkErrorShader(GLuint shader, const GLchar* text, GLuint status);
	void programInfoLog(GLuint shader);
	size_t push2DTexture(GLubyte3* image, GLuint width, GLuint height);
	bool pushShader(GLuint typeShader, GLchar* code, size_t length);
	size_t pushProgram();

	std::vector<GLuint>::iterator pushBuffer(void* data, size_t number_objects, size_t length_object, size_t typeBuffer);

	void pushDrawObjects(size_t number_objects);

	void setMouseOldPosition(int2 data);

	void setMouseNewPosition(int2 data);

	mat4x4* getViewMatrix();

	mat4x4* getProjectionMatrix();

	float4* getUpView();

	float4* getCenterView();

	float4* getEyeView();
	void cameraRotate();
	void Render();

	mat4x4* getModelMatrix();

	OpenGL_device(HDC hdc);

	OpenGL_device();
	~OpenGL_device();
};
