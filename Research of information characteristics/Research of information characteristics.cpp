#include <Windows.h>
#include <stdio.h>

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
GLuint program;
GLuint data_buffer;
GLuint color_buffer;
GLuint indices_buffer;
GLuint number_objects;
GLuint width = 200;
GLuint height = 200;
GLfloat near_draw_x = -1.0f;
GLfloat near_draw_y = -1.0f;
GLfloat near_draw_z = 0.0001f;
GLfloat far_draw_x = 1.0f;
GLfloat far_draw_y = 1.0f;
GLfloat far_draw_z = 100.0f;
GLfloat projectionMatrix[16];
GLfloat viewMatrix[16];
GLfloat modelMatrix[16];
GLfloat Up[3];
GLfloat Center[3];
GLfloat Eye[3];
GLint projection_matrix_index;
GLint model_matrix_index;
GLint view_matrix_index;
int main()
{
	for (size_t i = 0; i < 4; i++)
		for (size_t j = 0; j < 4; j++)
			if (i == j)
				projectionMatrix[i * 4 + j] = 1,
				modelMatrix[i * 4 + j] = 1,
				viewMatrix[i * 4 + j] = 1;
			else
				projectionMatrix[i * 4 + j] = 0,
				modelMatrix[i * 4 + j] = 0,
				viewMatrix[i * 4 + j] = 0;
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
	glPointSize(15.0f);
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
//GLfloat function(float x, float y) {
//	return 1.0f - x * x + y * y;
//}
GLfloat function(float x, float y) {
	return  x * x + y * y;
}
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
void cross_vec3(GLfloat* matrix_1, GLfloat* matrix_2, GLfloat* result) {
	result[0] = matrix_1[1] * matrix_2[2] - matrix_1[2] * matrix_1[1];
	result[1] = matrix_1[2] * matrix_2[0] - matrix_1[0] * matrix_1[2];
	result[2] = matrix_1[0] * matrix_2[1] - matrix_1[1] * matrix_1[0];
}
void normalize_vec3(GLfloat* matrix, GLfloat* result) {
	GLfloat vector = sqrt((matrix[0] * matrix[0]) + (matrix[1] * matrix[1]) + (matrix[2] * matrix[2]));
	result[0] = matrix[0] / vector;
	result[1] = matrix[1] / vector;
	result[2] = matrix[2] / vector;
}
void rotate(GLfloat* modelView, GLfloat angle, GLfloat* vector) {
	GLfloat cosine = cos(angle);
	GLfloat sine = sin(angle);
	normalize_vec3(vector, vector);
	GLfloat temp_x = (1.0f - cosine) * vector[0];
	GLfloat temp_y = (1.0f - cosine) * vector[1];
	GLfloat temp_z = (1.0f - cosine) * vector[2];

	GLfloat Rotate[4][4];
	GLfloat Result[16];
	Rotate[0][0] = cosine + temp_x * vector[0];
	Rotate[0][1] = temp_x * vector[1] + sine * vector[2];
	Rotate[0][2] = temp_x * vector[2] - sine * vector[1];

	Rotate[1][0] = temp_y * vector[0] - sine * vector[2];
	Rotate[1][1] = cosine + temp_y * vector[1];
	Rotate[1][2] = temp_y * vector[2] + sine * vector[0];

	Rotate[2][0] = temp_z * vector[0] + sine * vector[1];
	Rotate[2][1] = temp_z * vector[1] - sine * vector[0];
	Rotate[2][2] = cosine + temp_z * vector[2];

	Result[0 * 4] = modelView[0 * 4] * Rotate[0][0] + modelView[1 * 4] * Rotate[0][1] + modelView[2 * 4] * Rotate[0][2];
	Result[0 * 4 + 1] = modelView[0 * 4 + 1] * Rotate[0][0] + modelView[1 * 4 + 1] * Rotate[0][1] + modelView[2 * 4 + 1] * Rotate[0][2];
	Result[0 * 4 + 2] = modelView[0 * 4 + 2] * Rotate[0][0] + modelView[1 * 4 + 2] * Rotate[0][1] + modelView[2 * 4 + 2] * Rotate[0][2];
	Result[0 * 4 + 3] = modelView[0 * 4 + 3] * Rotate[0][0] + modelView[1 * 4 + 3] * Rotate[0][1] + modelView[2 * 4 + 3] * Rotate[0][2];
	Result[1 * 4] = modelView[0 * 4] * Rotate[1][0] + modelView[1 * 4] * Rotate[1][1] + modelView[2 * 4] * Rotate[1][2];
	Result[1 * 4 + 1] = modelView[0 * 4 + 1] * Rotate[1][0] + modelView[1 * 4 + 1] * Rotate[1][1] + modelView[2 * 4 + 1] * Rotate[1][2];
	Result[1 * 4 + 2] = modelView[0 * 4 + 2] * Rotate[1][0] + modelView[1 * 4 + 2] * Rotate[1][1] + modelView[2 * 4 + 2] * Rotate[1][2];
	Result[1 * 4 + 3] = modelView[0 * 4 + 3] * Rotate[1][0] + modelView[1 * 4 + 3] * Rotate[1][1] + modelView[2 * 4 + 3] * Rotate[1][2];
	Result[2 * 4] = modelView[0 * 4] * Rotate[2][0] + modelView[1 * 4] * Rotate[2][1] + modelView[2 * 4] * Rotate[2][2];
	Result[2 * 4 + 1] = modelView[0 * 4 + 1] * Rotate[2][0] + modelView[1 * 4 + 1] * Rotate[2][1] + modelView[2 * 4 + 1] * Rotate[2][2];
	Result[2 * 4 + 2] = modelView[0 * 4 + 2] * Rotate[2][0] + modelView[1 * 4 + 2] * Rotate[2][1] + modelView[2 * 4 + 2] * Rotate[2][2];
	Result[2 * 4 + 3] = modelView[0 * 4 + 3] * Rotate[2][0] + modelView[1 * 4 + 3] * Rotate[2][1] + modelView[2 * 4 + 3] * Rotate[2][2];
	Result[3 * 4] = modelView[3 * 4];
	Result[3 * 4 + 1] = modelView[3 * 4 + 1];
	Result[3 * 4 + 2] = modelView[3 * 4 + 2];
	Result[3 * 4 + 3] = modelView[3 * 4 + 3];
	for (size_t i = 0; i < 16; i++)
		modelView[i] = Result[i];
}
void lookAt(GLfloat* matrix) {
	float fx = Center[0] - Eye[0];
	float fy = Center[1] - Eye[1];
	float fz = Center[2] - Eye[2];
	float normalizeVector = 1.0f / sqrtf(fx*fx + fy * fy + fz * fz);
	fx *= normalizeVector;
	fy *= normalizeVector;
	fz *= normalizeVector;
	float sx = fy * Up[2] - fz * Up[1];
	float sy = fz * Up[0] - fx * Up[2];
	float sz = fx * Up[1] - fy * Up[0];
	normalizeVector = 1.0f / sqrtf(sx*sx + sy * sy + sz * sz);
	sx *= normalizeVector;
	sy *= normalizeVector;
	sz *= normalizeVector;
	if ((0 == sx) && (0 == sy) && (0 == sz))
		return;
	float ux = sy * fz - sz * fy;
	float uy = sz * fx - sx * fz;
	float uz = sx * fy - sy * fx;
	matrix[0] = sx;
	matrix[1] = ux;
	matrix[2] = -fx;
	matrix[3] = 0.0f;
	matrix[4] = sy;
	matrix[5] = uy;
	matrix[6] = -fy;
	matrix[7] = 0.0f;
	matrix[8] = sz;
	matrix[9] = uz;
	matrix[10] = -fz;
	matrix[11] = 0.0f;
	matrix[12] = 0.0f;
	matrix[13] = 0.0f;
	matrix[14] = 0.0f;
	matrix[15] = 1.0f;
	matrix[12] += matrix[0] * -Eye[0] + matrix[4] * -Eye[1] +
		matrix[8] * -Eye[2];
	matrix[13] += matrix[1] * -Eye[0] + matrix[5] * -Eye[1] +
		matrix[9] * -Eye[2];
	matrix[14] += matrix[2] * -Eye[0] + matrix[6] * -Eye[1] +
		matrix[10] * -Eye[2];
	matrix[15] += matrix[3] * -Eye[0] + matrix[7] * -Eye[1] +
		matrix[11] * -Eye[2];

}
void genModel(GLfloat* modelView, GLfloat angle, GLfloat* vector) {
	rotate(modelMatrix, angle, vector);
}
void genProjection(GLfloat width, GLfloat height, GLfloat z_near, GLfloat z_far, GLfloat FOV) {
	GLfloat aspect = width / height;
	GLfloat depth = z_far - z_near;
	GLfloat tanFOV = tan(FOV * 0.5f);
	projectionMatrix[0] = 1.0f / (aspect * tanFOV);
	projectionMatrix[5] = 1.0f / tanFOV;
	projectionMatrix[10] = -(z_far + z_near) / depth;
	projectionMatrix[11] = -1.0f;
	projectionMatrix[14] = -2.0f * (z_far * z_near) / depth;
}
void genModelView(GLfloat* matrix) {
	lookAt(matrix);
	/*float fx = Center[0] - Eye[0];
	float fy = Center[1] - Eye[1];
	float fz = Center[2] - Eye[2];
	float normalizeVector = 1.0f / sqrtf(fx*fx + fy * fy + fz * fz);
	fx *= normalizeVector;
	fy *= normalizeVector;
	fz *= normalizeVector;
	float sx = fy * Up[2] - fz * Up[1];
	float sy = fz * Up[0] - fx * Up[2];
	float sz = fx * Up[1] - fy * Up[0];
	normalizeVector = 1.0f / sqrtf(sx*sx + sy * sy + sz * sz);
	sx *= normalizeVector;
	sy *= normalizeVector;
	sz *= normalizeVector;
	if ((0 == sx) && (0 == sy) && (0 == sz))
		return;
	float ux = sy * fz - sz * fy;
	float uy = sz * fx - sx * fz;
	float uz = sx * fy - sy * fx;
	matrix[0] = sx;
	matrix[1] = ux;
	matrix[2] = -fx;
	matrix[3] = 0.0f;
	matrix[4] = sy;
	matrix[5] = uy;
	matrix[6] = -fy;
	matrix[7] = 0.0f;
	matrix[8] = sz;
	matrix[9] = uz;
	matrix[10] = -fz;
	matrix[11] = 0.0f;
	matrix[12] = 0.0f;
	matrix[13] = 0.0f;
	matrix[14] = 0.0f;
	matrix[15] = 1.0f;
	matrix[12] += matrix[0] * -Eye[0] + matrix[4] * -Eye[1] +
		matrix[8] * -Eye[2];
	matrix[13] += matrix[1] * -Eye[0] + matrix[5] * -Eye[1] +
		matrix[9] * -Eye[2];
	matrix[14] += matrix[2] * -Eye[0] + matrix[6] * -Eye[1] +
		matrix[10] * -Eye[2];
	matrix[15] += matrix[3] * -Eye[0] + matrix[7] * -Eye[1] +
		matrix[11] * -Eye[2];*/
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	GLfloat vector[3] = { 0.0f, 0.0, 1.0f };
	HDC hDC;
	PAINTSTRUCT ps;
	switch (uMsg) {
	case WM_CREATE: {
		Center[0] = 0;
		Center[1] = 0;
		Center[2] = 0;
		Eye[0] = -1.0f;
		Eye[1] = -1.0f;
		Eye[2] = 1.0f;
		Up[0] = 0.0;
		Up[1] = 0.0f;
		Up[2] = 1.0;	
		GLfloat cameraDirection[3];
		GLfloat cameraRight[3];
		normalize_vec3(Eye, cameraDirection);
		cross_vec3(cameraDirection, Up, cameraRight);
		normalize_vec3(cameraRight, cameraRight);
		cross_vec3(cameraRight, cameraDirection, Up);
		lookAt(modelMatrix);
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
		RECT rect;
		GetWindowRect(hWnd, &rect);
		glViewport(0, 0, rect.right - rect.left, rect.bottom - rect.top);
		genProjection(rect.right - rect.left, rect.bottom - rect.top, -0.001f, 100.0f, 45.0f);
		break;
	}
	case WM_PAINT: {

		GLfloat angle =0;
		rotate(modelMatrix, angle, vector);
		hDC = BeginPaint(hWnd, &ps);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glEnableClientState(GL_INDEX_ARRAY);
		glUniformMatrix4fv(projection_matrix_index, 1, GL_FALSE, projectionMatrix);
		glUniformMatrix4fv(view_matrix_index, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(model_matrix_index, 1, GL_FALSE, modelMatrix);
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