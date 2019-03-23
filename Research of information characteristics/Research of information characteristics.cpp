#include <Windows.h>
#include <stdint.h>
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
	void operator =(uint2 data) {
		x = data.x, y = data.y;
	}
	uint2 operator -(uint2 data) {
		return uint2{ x - data.x, y - data.y };
	}
};
struct vec4 {
	GLfloat x, y, z, w;
	vec4() {
		x = 0.0f; y = 0.0f; z = 0.0f; w = 0.0f;
	}
	vec4(GLfloat _x, GLfloat _y, GLfloat _z, GLfloat _w) {
		x = _x; y = _y; z = _z; w = _w;
	}
	GLfloat	operator +() {
		return x + y + z + w;
	}
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
		x = data.x; y = data.y; z = data.z; w = data.w;
		return data;
	}
};
struct mat4x4 {
	vec4 x, y, z, w;
	mat4x4() {
		x = { 1.0f, 0.0f, 0.0f, 0.0f };
		y = { 0.0f,1.0f,0.0f,0.0f };
		z = { 0.0f,0.0f,1.0f,0.0f };
		w = { 0.0f,0.0f,0.0f,1.0f };
	}
	mat4x4(vec4 _x, vec4 _y, vec4 _z, vec4 _w) {
		x = _x, y = _y, z = _z, w = _w;
	}
	mat4x4 operator *(mat4x4 data) {
		vec4 _x = vec4{ x.x, y.x, z.x, w.x };
		vec4 _y = vec4{ x.y, y.y, z.y, w.y };
		vec4 _z = vec4{ x.z, y.z, z.z, w.z };
		vec4 _w = vec4{ x.w, y.w, z.w, w.w };
		vec4 x_x = data.x * _x;
		vec4 x_y = data.x * _y;
		vec4 x_z = data.x * _z;
		vec4 x_w = data.x * _w;
		vec4 y_x = data.y * _x;
		vec4 y_y = data.y * _y;
		vec4 y_z = data.y * _z;
		vec4 y_w = data.y * _w;
		vec4 z_x = data.z * _x;
		vec4 z_y = data.z * _y;
		vec4 z_z = data.z * _z;
		vec4 z_w = data.z * _w;
		vec4 w_x = data.w * _x;
		vec4 w_y = data.w * _y;
		vec4 w_z = data.w * _z;
		vec4 w_w = data.w * _w;
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
#define ID_BUTTON_1 1000
#define ID_BUTTON_2 1001
#define ID_BUTTON_3 1002
struct transferData {
	GLuint program;
	GLuint data_buffer;
	GLuint color_buffer;
	GLuint indices_buffer;
	GLuint number_objects;
	mat4x4 projectionMatrix;
	mat4x4 viewMatrix;
	mat4x4 modelMatrix;
	vec4 Up = { 0.0f, 1.0f, 0.0f, 0.0f };
	vec4 Center = { 0.0f, 0.0f, 0.0f, 0.0f };
	vec4 Eye = { 0.0f,0.0f,1.0f,0.0f };
	uint32_t mouse_wheel;
	uint2 new_mouse_position;
	uint2 old_mouse_position;
	GLint projection_matrix_index;
	GLint model_matrix_index;
	GLint view_matrix_index;
};
int main()
{
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
	HMENU hMenu = CreateMenu();
	AppendMenu(hMenu, MF_STRING, ID_BUTTON_1, "DrawFunction");
	AppendMenu(hMenu, MF_STRING, ID_BUTTON_2, "DrawHistogram");
	AppendMenu(hMenu, MF_STRING, ID_BUTTON_3, "&Edit");
	AppendMenu(hMenu, MF_STRING, 0, "&Help");
	SetMenu(hwnd, hMenu);
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
	glPointSize(5.0f);
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
GLfloat* genPositions(GLint width, GLint height, GLfloat& max, GLfloat& min, GLfloat far_draw_x, GLfloat near_draw_x, GLfloat far_draw_y, GLfloat near_draw_y) {
	GLfloat* position = (GLfloat*)malloc(width * height * 3 * sizeof(GLfloat));
	GLint start_h = -height / 2;
	GLint start_w = -width / 2;
	GLfloat step_x = (far_draw_x - near_draw_x) / width;
	GLfloat step_y = (far_draw_y - near_draw_y) / height;

	for (GLint y = start_h, _y = 0; _y < height; y++, _y++) {
		for (GLint x = start_w, _x = 0; _x < width; x++, _x++) {
			GLfloat f_x = x ;
			GLfloat f_y = y ;
			GLfloat f_z = function(x, y);
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
	GLfloat step = 1.0f / (max - min);
	GLfloat radius = 1.0f;
	GLfloat normalize = (2.0f * 3.1416f * radius) / max;
	for (GLint i = 0; i < length; i++) {
		GLfloat position = (min + positions[i * 3 + 2]) * step;
		color[i * 3] = position;
		color[i * 3 + 1] = position;
		color[i * 3 + 2] = position;
	}
	return color;
}
GLuint* genIndices(GLint width, GLint height) {
	GLuint* index = (GLuint*)malloc(width * height * sizeof(GLuint));
	for (GLint i = 0; i < width * height; i++) {
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
mat4x4 rotate(mat4x4& modelView, GLfloat angle, vec4 vector) {
	GLfloat cosine = cosf(angle);
	GLfloat sine = sinf(angle);
	vector = normalize_vec3(vector);
	vec4 temp = vector * (1.0f - cosine);
	vec4 Rotate[3];
	mat4x4 Result;
	vec4 angle_data = vec4{ cosine , sine * vector.z, -sine * vector.y, 0.0f };
	Rotate[0] = vector * temp.x + angle_data;
	angle_data = vec4{ -sine * vector.z , cosine, sine * vector.x, 0.0f };
	Rotate[1] = vector * temp.y + angle_data;
	angle_data = vec4{ sine * vector.y ,  -sine * vector.x,cosine, 0.0f };
	Rotate[2] = vector * temp.z + angle_data;
	Result.x = modelView.x * Rotate[0].x + modelView.y * Rotate[0].y + modelView.z * Rotate[0].z;
	Result.y = modelView.x * Rotate[1].x + modelView.y * Rotate[1].y + modelView.z * Rotate[1].z;
	Result.z = modelView.x * Rotate[2].x + modelView.y * Rotate[2].y + modelView.z * Rotate[2].z;
	return Result;
}
void lookAt(mat4x4& matrix, vec4 Eye, vec4 Up, vec4 Center) {
	vec4 z = Eye - Center;
	z = normalize_vec3(z);
	vec4 x = cross_vec3(Up, z);
	x = normalize_vec3(x);
	vec4 y = cross_vec3(z, x);
	y = normalize_vec3(y);
	matrix.x = { x.x, y.x, z.x, 0.0f };
	matrix.y = { x.y, y.y, z.y, 0.0f };
	matrix.z = { x.z, y.z, z.z, 0.0f };
	matrix.w = { 0.0f, 0.0f, 0.0f, 1.0f };
}
void genProjection(mat4x4& projectionMatrix, GLfloat width, GLfloat height, GLfloat z_near, GLfloat z_far, GLfloat FOV) {
	GLfloat aspect = width / height;
	GLfloat depth = z_far - z_near;
	GLfloat tanFOV = tan(FOV * 0.5f);
	projectionMatrix.x.x = 1.0f / (aspect * tanFOV);
	projectionMatrix.y.y = 1.0f / tanFOV;
	projectionMatrix.z.z = -(z_far + z_near) / depth;
	projectionMatrix.z.w = 2.0f * (z_far * z_near) / depth;
	projectionMatrix.w.z = -1.0f;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static transferData* data;
	HDC hDC;
	PAINTSTRUCT ps;
	if (uMsg == WM_CREATE) {
		RECT rect;
		GetWindowRect(hWnd, &rect);
		data = (transferData *) new transferData[1];
		data->old_mouse_position = { 0 ,0 };
		vec4 cameraRight;
		vec4 cameraDirection = normalize_vec3(data->Eye);
		cameraRight = cross_vec3(data->Up, cameraDirection);
		cameraRight = normalize_vec3(cameraRight);
		data->Up = cross_vec3(cameraRight, cameraDirection);
		lookAt(data->modelMatrix, data->Eye, data->Up, data->Center);
		HMODULE hInstance = GetModuleHandle(NULL);
	}
	switch (uMsg) {
	case WM_COMMAND:
		switch (wParam) {
		case ID_BUTTON_1: {
			if (data->data_buffer)
				glDeleteBuffers(1, &data->data_buffer);
			if (data->color_buffer)
				glDeleteBuffers(1, &data->color_buffer);
			if (data->indices_buffer)
				glDeleteBuffers(1, &data->indices_buffer);
			GLfloat max = 0.0f, min = MAXDWORD64;
			GLfloat* positions = genPositions(128, 128, max, min, -1.2f, 1.2f, -1.2f, 1.2f);
			GLfloat* colors = genColors(positions, max, min, 128 * 128);
			GLuint* indices = genIndices(128, 128);
			glGenBuffers(1, &data->data_buffer);
			glBindBuffer(GL_ARRAY_BUFFER, data->data_buffer);
			glBufferData(GL_ARRAY_BUFFER, data->number_objects * 3 * sizeof(GLfloat), positions, GL_STATIC_DRAW);
			glGenBuffers(1, &data->color_buffer);
			glBindBuffer(GL_ARRAY_BUFFER, data->color_buffer);
			glBufferData(GL_ARRAY_BUFFER, data->number_objects * 3 * sizeof(GLfloat), colors, GL_STATIC_DRAW);
			glGenBuffers(1, &data->indices_buffer);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data->indices_buffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, data->number_objects * sizeof(GLuint), indices, GL_STATIC_DRAW);
			GLint vert_index = glGetAttribLocation(data->program, "positions");
			GLint color_index = glGetAttribLocation(data->program, "colors");
			data->projection_matrix_index = glGetUniformLocation(data->program, "projectionMatrix");
			data->model_matrix_index = glGetUniformLocation(data->program, "modelMatrix");
			data->view_matrix_index = glGetUniformLocation(data->program, "viewMatrix");
			glBindBuffer(GL_ARRAY_BUFFER, data->data_buffer);
			glEnableVertexAttribArray(vert_index);
			glVertexAttribPointer(vert_index, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glBindBuffer(GL_ARRAY_BUFFER, data->color_buffer);
			glEnableVertexAttribArray(color_index);
			glVertexAttribPointer(color_index, 3, GL_FLOAT, GL_FALSE, 0, 0);
			free(colors);
			free(positions);
			free(indices);
			data->number_objects = 128 * 128;
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
		case ID_BUTTON_2: {
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
		}
	case WM_SHOWWINDOW: {
		data->number_objects = 0;
		GLint infoLength = 0;

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

		data->program = glCreateProgram();
		glAttachShader(data->program, vertexShader);
		glAttachShader(data->program, fragmentShader);
		glLinkProgram(data->program);
		glGetProgramiv(data->program, GL_INFO_LOG_LENGTH, &infoLength);
		if (infoLength) {
			GLchar* infoLog = (GLchar*)malloc(infoLength * sizeof(GLchar));
			glGetProgramInfoLog(data->program, infoLength, NULL, infoLog);
			MessageBox(NULL, infoLog, "Error", MB_OK);
			free(infoLog);
		}
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		glUseProgram(data->program);

		//data->modelMatrix = rotate(data->modelMatrix, 0.5, vec4(1, 0, 0, 0));
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	}
	case WM_KEYDOWN:
	{
		if (wParam == VK_LEFT) {
			data->Eye.x += 0.01f;
		}
		else 	if (wParam == VK_RIGHT) {
			data->Eye.x += -0.01f;
		}
		else if (wParam == VK_UP) {
			data->Eye.z += 0.01f;
		}
		else if (wParam == VK_DOWN) {
			data->Eye.z += -0.01f;
		}
		//vec4 cameraRight;
		//vec4 cameraDirection = normalize_vec3(data->Eye);
		//cameraRight = cross_vec3(data->Up, cameraDirection);
		//cameraRight = normalize_vec3(cameraRight);
		//data->Up = cross_vec3(cameraRight, cameraDirection);
		lookAt(data->modelMatrix, data->Eye, data->Up, data->Center);
		break;
	}
	case WM_SIZING: {
	}
	case WM_SIZE: {
		RECT rect;
		GetWindowRect(hWnd, &rect);
		glViewport(0, 0, rect.right - rect.left, rect.bottom - rect.top);
		genProjection(data->projectionMatrix, rect.right - rect.left, rect.bottom - rect.top, 1.0f, -1.0f, 45.0f);
		InvalidateRect(hWnd, 0, TRUE);
		break;
	}
	case WM_PAINT: {
		//GLfloat angle = GetTickCount64() / 1000000000.0f;
		//data->modelMatrix = rotate(data->modelMatrix, 0.01, vec4(0, 0, 1, 0));
		hDC = BeginPaint(hWnd, &ps);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glEnableClientState(GL_INDEX_ARRAY);
		glUniformMatrix4fv(data->projection_matrix_index, 1, GL_FALSE, (GLfloat*)&data->projectionMatrix);
		glUniformMatrix4fv(data->view_matrix_index, 1, GL_FALSE, (GLfloat*)&data->viewMatrix);
		glUniformMatrix4fv(data->model_matrix_index, 1, GL_FALSE, (GLfloat*)&data->modelMatrix);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data->indices_buffer);
		glDrawElements(GL_POINTS, data->number_objects, GL_UNSIGNED_INT, NULL);
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