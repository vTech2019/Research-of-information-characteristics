
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
GLuint data_buffer;
GLuint color_buffer;
GLuint width = 400;
GLuint height = 400;
int main()
{
	HMODULE Instance = GetModuleHandle(NULL);
	HWND hwnd;
	MSG msg;
	WNDCLASS wc = { 0 };
	wc.hInstance = Instance;
	wc.lpfnWndProc = WndProc;
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszClassName = L"MainWindow";
	RegisterClass(&wc);
	hwnd = CreateWindow(L"MainWindow", 	L"OpenGL", 	WS_OVERLAPPEDWINDOW,	CW_USEDEFAULT,	NULL, CW_USEDEFAULT, 	NULL, (HWND)NULL, NULL, HINSTANCE(Instance), 	NULL);
	HDC hdc = GetDC(hwnd);
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),   
		1,              
		PFD_DRAW_TO_WINDOW |  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,   
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
	INT iPixelFormat;
	iPixelFormat = ChoosePixelFormat(hdc, &pfd);
	SetPixelFormat(hdc, iPixelFormat, &pfd);
	HGLRC gl_context = wglCreateContext(hdc);
	wglMakeCurrent(hdc, gl_context);

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);
	while (GetMessage(&msg, NULL, NULL, NULL)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
	}
	return msg.wParam;
}
void genData(float* position, GLint width, GLint height) {
	INT start_h = -height / 2;
	INT start_w = -width / 2;
	for (INT h = start_h; h < height / 2; h++) {
		for (INT w = start_w; w < width / 2; w++) {
			position[(h * width + w) * 3] =
			position[(h * width + w) * 3 + 1] =
			position[(h * width + w) * 3 + 2] =
		}
	}
}
LRESULT CALLBACK WndProc(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	HDC hDC;
	PAINTSTRUCT ps;
	switch (uMsg) {
	case WM_CREATE:
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &codeVertexShader, NULL);
		glCompileShader(vertexShader);
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &codeFragmentShader, NULL);
		glCompileShader(fragmentShader);

		const GLuint program = glCreateProgram();
		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);
		glLinkProgram(program);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		glUseProgram(program);

		glGenBuffers(1, &data_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, data_buffer);
		glBufferData(GL_ARRAY_BUFFER, width * height * 3 * sizeof(float), position, GL_STATIC_DRAW);
		glGenBuffers(1, &color_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
		glBufferData(GL_ARRAY_BUFFER, width * height * 3 * sizeof(float), color, GL_STATIC_DRAW);
		break;
	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
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