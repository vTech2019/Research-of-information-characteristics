#include <Windows.h>
#define GLEW_STATIC
#include "glew-2.1.0/include/GL/glew.h"
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
GLfloat near_draw_z = -1.0f;
GLfloat far_draw_x = 1.0f;
GLfloat far_draw_y = 1.0f;
GLfloat far_draw_z = 1.0f;
int main()
{
	HMODULE Instance = GetModuleHandle(NULL);
	HWND hwnd;
	MSG msg;
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

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);
	while (GetMessage(&msg, NULL, NULL, NULL)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
GLfloat function(float x, float y) {
	return 1.0f - x * x + y * y;
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
		GLfloat position = positions[i * 3 + 2] * step;
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
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hDC;
	PAINTSTRUCT ps;
	switch (uMsg) {
	case WM_SHOWWINDOW: {
		GLint infoLength = 0;
		GLfloat* positions = NULL;
		GLfloat* colors = NULL;
		GLuint* indices = NULL;
		GLfloat max = 0.0f, min = MAXDWORD64;
		number_objects = width * height;
		positions = genPositions(width, height, max, min);
		colors = genColors(positions, max, min, number_objects);
		indices = genIndices(number_objects);
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
			MessageBox(NULL, "Error", infoLog, MB_OK);
			free(infoLog);
		}

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &infoLength);
		if (infoLength == GL_FALSE) {
			glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &infoLength);
			GLchar* infoLog = (GLchar*)malloc(infoLength * sizeof(GLchar));
			glGetShaderInfoLog(fragmentShader, infoLength, NULL, infoLog);
			MessageBox(NULL, "Error", infoLog, MB_OK);
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
			MessageBox(NULL, "Error", infoLog, MB_OK);
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
		glBindBuffer(GL_ARRAY_BUFFER, data_buffer);
		glEnableVertexAttribArray(vert_index);
		glVertexAttribPointer(vert_index,  3 , GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
		glEnableVertexAttribArray(color_index);
		glVertexAttribPointer(color_index, 3 , GL_FLOAT, GL_FALSE, 0, 0);

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
		break;
	}
	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glEnableClientState(GL_INDEX_ARRAY);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_buffer);
		glDrawElements(GL_POINTS, number_objects, GL_UNSIGNED_INT, NULL);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_INDEX_ARRAY);
		SwapBuffers(hDC);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}