#include <Windows.h>
#include <stdio.h>
#include "glm/glm/glm.hpp"
#include "glm/glm/matrix.hpp"
#define GLEW_STATIC
#include "glew-2.1.0/include/GL/glew.h"
#define _USE_MATH_DEFINES
#include <math.h>
#ifndef _WIN64
#pragma comment(lib, "glew-2.1.0/lib/Release/Win32/glew32s.lib")
#pragma comment(lib, "opengl32.lib")
#else
#pragma comment(lib, "glew-2.1.0/lib/Release/x64/glew32s.lib")
#pragma comment(lib, "opengl32.lib")
#endif
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

const char* codeVertexShader =
#include "vertexShader.glsl"
;
const char* codeFragmentShader =
#include "fragmentShader.glsl"
;
struct uint2 {
	uint32_t x, y;
	uint2 operator -(uint2 data) {
		return uint2{ x - data.x, y - data.y };
	}
};
struct vec4 {
	GLfloat x, y, z, w;
	vec4 operator +(vec4 data) {
		return vec4{ x + data.x, y + data.y, z + data.z,w + data.w };
	}
	vec4 operator *(GLfloat data) {
		return vec4{ x * data, y * data, z * data,w * data };
	}
	vec4 operator *(vec4 data) {
		return vec4{ x * data.x, y * data.y, z * data.z,w * data.w };
	}
	vec4 operator -(vec4 data) {
		return vec4{ x - data.x, y - data.y, z - data.z,w - data.w };
	}
	vec4 operator =(vec4 data) {
		return vec4{ x = data.x, y = data.y, z = data.z,w = data.w };
	}
};
GLuint program;
GLuint data_buffer;
GLuint color_buffer;
GLuint indices_buffer;
GLuint number_objects;
GLuint width = 2000;
GLuint height = 2000;
GLfloat near_draw_x = -1.0f;
GLfloat near_draw_y = -1.0f;
GLfloat near_draw_z = 0.0001f;
GLfloat far_draw_x = 1.0f;
GLfloat far_draw_y = 1.0f;
GLfloat far_draw_z = 100.0f;
vec4 projectionMatrix[4];
vec4 viewMatrix[4];
vec4 modelMatrix[4];
vec4 Up;
vec4 Center;
vec4 Eye;
GLint projection_matrix_index;
GLint model_matrix_index;
GLint view_matrix_index;
uint32_t mouse_wheel;
uint2 new_mouse_position;
uint2 old_mouse_position;
int main()
{
	mouse_wheel = 0;
	projectionMatrix[0] = vec4{ 1.0f, 0.0f, 0.0f, 0.0f };
	modelMatrix[0] = vec4{ 1.0f, 0.0f, 0.0f, 0.0f };
	viewMatrix[0] = vec4{ 1.0f, 0.0f, 0.0f, 0.0f };
	projectionMatrix[1] = vec4{ 0.0f, 1.0f, 0.0f, 0.0f };
	modelMatrix[1] = vec4{ 0.0f, 1.0f, 0.0f, 0.0f };
	viewMatrix[1] = vec4{ 0.0f, 1.0f, 0.0f, 0.0f };
	projectionMatrix[2] = vec4{ 0.0f, 0.0f, 1.0f, 0.0f };
	modelMatrix[2] = vec4{ 0.0f, 0.0f, 1.0f, 0.0f };
	viewMatrix[2] = vec4{ 0.0f, 0.0f, 1.0f, 0.0f };
	projectionMatrix[3] = vec4{ 0.0f, 0.0f, 0.0f, 1.0f };
	modelMatrix[3] = vec4{ 0.0f, 0.0f, 0.0f, 1.0f };
	viewMatrix[3] = vec4{ 0.0f, 0.0f, 0.0f, 1.0f };

	HMODULE Instance = GetModuleHandle(NULL);
	HWND hwnd;
	MSG msg = { 0 };
	WNDCLASS wc = { 0 };
	wc.hInstance = Instance;
	wc.lpfnWndProc = WndProc;
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.hCursor = (HCURSOR)LoadCursor(Instance, IDC_ARROW);
	wc.lpszClassName = "MainWindow";
	RegisterClass(&wc);
	hwnd = CreateWindow("MainWindow", "OpenGL", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, NULL, CW_USEDEFAULT, NULL, (HWND)NULL, NULL, HINSTANCE(Instance), NULL);
	HDC hdc = GetDC(hwnd);
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		24,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		32,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};

	SetPixelFormat(hdc, ChoosePixelFormat(hdc, &pfd), &pfd);
	HGLRC gl_context = wglCreateContext(hdc);
	wglMakeCurrent(hdc, gl_context);
	if (glewInit() != GLEW_OK)
		MessageBox(NULL, "Error", "Error init glew!", MB_OK);
	glewExperimental = GL_TRUE;
	//glPointSize(15.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			InvalidateRect(hwnd, NULL, FALSE);
		}
	}
	return msg.wParam;
}
GLfloat function(float x, float y) {
	return 1.0f - x * x + y * y;
}
//GLfloat function(float x, float y) {
//	return  x * x + y * y;
//}
GLfloat* genPositions(GLint width, GLint height, GLfloat& max, GLfloat& min) {
	GLfloat* position = (GLfloat*)malloc(width * height * 3 * sizeof(GLfloat));
	GLint start_h = -height / 2;
	GLint start_w = -width / 2;
	GLfloat step_x = (far_draw_x - near_draw_x) / width;
	GLfloat step_y = (far_draw_y - near_draw_y) / height;

	for (GLint y = start_h, _y = 0; _y < height; y++, _y++) {
		for (GLint x = start_w, _x = 0; _x < width; x++, _x++) {
			GLfloat f_x = x * step_x;
			GLfloat f_y = y * step_y;
			GLfloat f_z = function(f_x, f_y);
			max = max < f_z ? f_z : max;
			min = min < f_z ? min : f_z;
			position[(_y * width + _x) * 3] = f_x;
			position[(_y * width + _x) * 3 + 1] = f_y;
			position[(_y * width + _x) * 3 + 2] = f_z;
		}
	}
	return position;
}
GLfloat* genColors(GLfloat* positions, GLfloat max, GLfloat min, GLint length) {
	GLfloat* color = (GLfloat*)malloc(length * 3 * sizeof(GLfloat));
	GLfloat step = 3.0f / (max - min);
	for (GLuint i = 0; i < length; i++) {
		GLfloat position = (min + positions[i * 3 + 2]) * step;
		GLfloat red = position - 2.0f < 0.0f ? 0.0f : position - 2.0f;
		GLfloat green = position - red - 1.0f < 0.0f ? 0.0f : position - red - 1.0f;
		GLfloat blue = position > 1.0f ? 1.0f : position;
		color[i * 3] = red;
		color[i * 3 + 1] = green;
		color[i * 3 + 2] = blue;
	}
	return color;
}
GLuint* genIndices(GLint length) {
	GLuint* index = (GLuint*)malloc(length * sizeof(GLuint));
	for (GLuint i = 0; i < length; i++) {
		index[i] = i;
	}
	return index;
}
vec4 cross_vec3(vec4 matrix_1, vec4 matrix_2) {
	vec4 vector_1_1 = vec4{ matrix_1.y, matrix_1.z, matrix_1.x, 0.0f };
	vec4 vector_1_2 = vec4{ matrix_1.z, matrix_1.x, matrix_1.y, 0.0f };
	vec4 vector_2_1 = vec4{ matrix_2.z, matrix_2.x, matrix_2.y, 0.0f };
	vec4 vector_2_2 = vec4{ matrix_2.y, matrix_2.z, matrix_2.x, 0.0f };
	return vec4{ vector_1_1 * vector_2_1 - vector_1_2 * vector_2_2 };
}
vec4 normalize_vec3(vec4 matrix) {
	GLfloat vector = 1.0f / sqrtf((matrix.x * matrix.x) + (matrix.y * matrix.y) + (matrix.z * matrix.z));
	return matrix * vector;
}
void rotate(vec4* modelView, GLfloat angle, vec4 vector) {
	GLfloat cosine = cos(angle);
	GLfloat sine = sin(angle);
	vector = normalize_vec3(vector);
	vec4 temp = vector * (1.0f - cosine);
	vec4 Rotate[3];
	vec4 Result[3];
	vec4 angle_data = vec4{ cosine , sine * vector.z, -sine * vector.y, 0.0f };
	Rotate[0] = vector * temp.x + angle_data;
	angle_data = vec4{ -sine * vector.z , cosine, sine * vector.x, 0.0f };
	Rotate[1] = vector * temp.y + angle_data;
	angle_data = vec4{ sine * vector.y ,  -sine * vector.x,cosine, 0.0f };
	Rotate[2] = vector * temp.z + angle_data;
	Result[0] = modelView[0] * Rotate[0].x + modelView[1] * Rotate[0].y + modelView[2] * Rotate[0].z;
	Result[1] = modelView[0] * Rotate[1].x + modelView[1] * Rotate[1].y + modelView[2] * Rotate[1].z;
	Result[2] = modelView[0] * Rotate[2].x + modelView[1] * Rotate[2].y + modelView[2] * Rotate[2].z;
	modelView[0] = Result[0];
	modelView[1] = Result[1];
	modelView[2] = Result[2];
}
void lookAt(vec4* matrix) {
	vec4 z = Eye - Center;
	z = normalize_vec3(z);
	vec4 x = cross_vec3(Up, z);
	x = normalize_vec3(x);
	vec4 y = cross_vec3(z, x);
	y = normalize_vec3(y);
	matrix[0] = { x.x, y.x, z.x, 0.0f };
	matrix[1] = { x.y, y.y, z.y, 0.0f };
	matrix[2] = { x.z, y.z, z.z, 0.0f };
	matrix[3] = { 0.0f, 0.0f, 0.0f, 1.0f };
}
void genModel(vec4* modelView, GLfloat angle, vec4 vector) {
	rotate(modelMatrix, angle, vector);
}
void genProjection(GLfloat width, GLfloat height, GLfloat z_near, GLfloat z_far, GLfloat FOV) {
	GLfloat aspect = width / height;
	GLfloat depth = z_far - z_near;
	GLfloat tanFOV = tan(FOV * 0.5f);
	projectionMatrix[0].x = 1.0f / (aspect * tanFOV);
	projectionMatrix[1].y = 1.0f / tanFOV;
	projectionMatrix[2].z = -(z_far + z_near) / depth;
	projectionMatrix[2].w = 2.0f * (z_far * z_near) / depth;
	projectionMatrix[3].z = -1.0f;
}
void genModelView(vec4* matrix) {
	lookAt(matrix);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	vec4 vector = { 0.0f, 1.0, 0.0f, 0.0f };
	HDC hDC;
	PAINTSTRUCT ps;
	switch (uMsg) {
	case WM_CREATE: {
		Center = vec4{ 0.0f,0.0f,0.0f,0.0f };
		Eye = vec4{ 0.0f,0.0f,1.0f,0.0f };
		Up = vec4{ 0.0f,1.0f,0.0f,0.0f };
		vec4 cameraRight;
		vec4 cameraDirection = normalize_vec3(Eye);
		cameraRight = cross_vec3(Up, cameraDirection);
		cameraRight = normalize_vec3(cameraRight);
		Up = cross_vec3(cameraRight, cameraDirection);
		lookAt(modelMatrix);
		RECT rect;
		GetWindowRect(hWnd, &rect);
		genProjection(rect.right - rect.left, rect.bottom - rect.top, 100.0f, -100.0f, 45.0f);
		break;
	}
	case WM_SHOWWINDOW: {
		number_objects = width * height;
		GLint infoLength = 0;
		GLfloat max = 0.0f, min = MAXDWORD64;
		GLfloat* positions = genPositions(width, height, max, min);
		GLfloat* colors = genColors(positions, max, min, number_objects);
		GLuint* indices = genIndices(number_objects);
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &codeVertexShader, NULL);
		glCompileShader(vertexShader);
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &codeFragmentShader, NULL);
		glCompileShader(fragmentShader);

		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &infoLength);
		if (infoLength == GL_FALSE) {
			glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &infoLength);
			GLchar* infoLog = (GLchar*)malloc(infoLength * sizeof(GLchar));
			glGetShaderInfoLog(vertexShader, infoLength, NULL, infoLog);
			MessageBox(NULL, infoLog, "Error", MB_OK);
			free(infoLog);
		}

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &infoLength);
		if (infoLength == GL_FALSE) {
			glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &infoLength);
			GLchar* infoLog = (GLchar*)malloc(infoLength * sizeof(GLchar));
			glGetShaderInfoLog(fragmentShader, infoLength, NULL, infoLog);
			MessageBox(NULL, infoLog, "Error", MB_OK);
			free(infoLog);
		}

		program = glCreateProgram();
		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);
		glLinkProgram(program);
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLength);
		if (infoLength) {
			GLchar* infoLog = (GLchar*)malloc(infoLength * sizeof(GLchar));
			glGetProgramInfoLog(program, infoLength, NULL, infoLog);
			MessageBox(NULL, infoLog, "Error", MB_OK);
			free(infoLog);
		}
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		glUseProgram(program);

		glGenBuffers(1, &data_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, data_buffer);
		glBufferData(GL_ARRAY_BUFFER, number_objects * 3 * sizeof(GLfloat), positions, GL_STATIC_DRAW);
		glGenBuffers(1, &color_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
		glBufferData(GL_ARRAY_BUFFER, number_objects * 3 * sizeof(GLfloat), colors, GL_STATIC_DRAW);
		glGenBuffers(1, &indices_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, number_objects * sizeof(GLuint), indices, GL_STATIC_DRAW);
		GLint vert_index = glGetAttribLocation(program, "positions");
		GLint color_index = glGetAttribLocation(program, "colors");
		projection_matrix_index = glGetUniformLocation(program, "projectionMatrix");
		model_matrix_index = glGetUniformLocation(program, "modelMatrix");
		view_matrix_index = glGetUniformLocation(program, "viewMatrix");
		glBindBuffer(GL_ARRAY_BUFFER, data_buffer);
		glEnableVertexAttribArray(vert_index);
		glVertexAttribPointer(vert_index, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
		glEnableVertexAttribArray(color_index);
		glVertexAttribPointer(color_index, 3, GL_FLOAT, GL_FALSE, 0, 0);

		free(colors);
		free(positions);
		free(indices);
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	}
	case WM_SIZING: {
	}
	case WM_SIZE: {
		RECT rect;
		GetWindowRect(hWnd, &rect);
		glViewport(0, 0, rect.right - rect.left, rect.bottom - rect.top);
		genProjection(rect.right - rect.left, rect.bottom - rect.top, 100.0f, -100.0f, 45.0f);
		InvalidateRect(hWnd, 0, TRUE);
		break;
	}
	case WM_MOUSEWHEEL: {
		mouse_wheel += GET_WHEEL_DELTA_WPARAM(wParam) / 120;
		InvalidateRect(hWnd, 0, TRUE);
		break;
	}
	case WM_LBUTTONDOWN:
		break;
	case WM_MOUSEMOVE: {
		if (UINT(wParam) & MK_LBUTTON)
		{
			uint2 mouse_delta = new_mouse_position - old_mouse_position;
			if (sqrt(mouse_delta.x*mouse_delta.x + mouse_delta.y*mouse_delta.y) > 50) {
				old_mouse_position = new_mouse_position;
				break;
			}

				mouse_point.x = LOWORD(lParam);
			mouse_point.y = HIWORD(lParam);

			Center = vec4{ 0.0f,0.0f,0.0f,0.0f };
			Up = vec4{ 0.0f,1.0f,0.0f,0.0f };
			vec4 cameraDirection = normalize_vec3(Eye);
			vec4 cameraRight = cross_vec3(Up, cameraDirection);
			cameraRight = normalize_vec3(cameraRight);
			Up = cross_vec3(cameraRight, cameraDirection);
			lookAt(modelMatrix);
			InvalidateRect(hWnd, NULL, FALSE);
		}
		break;
	}
	case WM_PAINT: {
		GLfloat angle = GetTickCount64() / 1000000000.0f;
		rotate(modelMatrix, angle, vector);
		hDC = BeginPaint(hWnd, &ps);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glEnableClientState(GL_INDEX_ARRAY);
		glUniformMatrix4fv(projection_matrix_index, 1, GL_FALSE, (GLfloat*)projectionMatrix);
		glUniformMatrix4fv(view_matrix_index, 1, GL_FALSE, (GLfloat*)viewMatrix);
		glUniformMatrix4fv(model_matrix_index, 1, GL_FALSE, (GLfloat*)modelMatrix);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_buffer);
		glDrawElements(GL_POINTS, number_objects, GL_UNSIGNED_INT, NULL);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_INDEX_ARRAY);
		SwapBuffers(hDC);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}