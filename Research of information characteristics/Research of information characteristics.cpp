#include <time.h>

#include "Math.h"
#include "OpenGL_device.h"
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

const char codeVertexShader[] =
#include "vertexShader.glsl"
;
const char codeFragmentShader[] =
#include "fragmentShader.glsl"
;
enum HidUsagePage : uint16_t
{
	UNDEFINED = 0x00,
	GENERIC = 0x01,
	SIMULATION = 0x02,
	VR = 0x03,
	SPORT = 0x04,
	GAME = 0x05,
	KEYBOARD = 0x07,
};

enum HidUsage : uint16_t
{
	Undefined = 0x00,
	Pointer = 0x01,
	Mouse = 0x02,
	Joystick = 0x04,
	Gamepad = 0x05,
	Keyboard = 0x06,
	Keypad = 0x07,
	SystemControl = 0x80,
	Tablet = 0x80,
	Consumer = 0x0C,
};
#define ID_BUTTON_1 1000
#define ID_BUTTON_2 1001
#define ID_BUTTON_3 1002
#define ID_BUTTON_4 1003
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
	if (!RegisterClass(&wc))
		MessageBox(NULL, "Failed to register window", "Error!", MB_OK);
	hwnd = CreateWindow("MainWindow", "OpenGL", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, NULL, CW_USEDEFAULT, NULL, (HWND)NULL, NULL, HINSTANCE(Instance), NULL);

	RAWINPUTDEVICE Rid;
	Rid.usUsagePage = GENERIC;
	Rid.usUsage = Mouse;
	Rid.dwFlags = RIDEV_INPUTSINK;
	Rid.hwndTarget = hwnd;

	if (!RegisterRawInputDevices(&Rid, 1, sizeof(Rid))) {
		MessageBox(NULL, "Raw devices registration failed!", "Error!", MB_OK);
	}
	Rid.usUsage = Keyboard;
	if (!RegisterRawInputDevices(&Rid, 1, sizeof(Rid))) {
		MessageBox(NULL, "Raw devices registration failed!", "Error!", MB_OK);
	}
	HMENU hMenu = CreateMenu();
	AppendMenu(hMenu, MF_STRING, ID_BUTTON_1, "DrawFunction");
	AppendMenu(hMenu, MF_STRING, ID_BUTTON_2, "DrawHistogram");
	AppendMenu(hMenu, MF_STRING, ID_BUTTON_3, "&Edit");
	AppendMenu(hMenu, MF_STRING, ID_BUTTON_4, "&Help");
	SetMenu(hwnd, hMenu);

	OpenGL_device device(GetDC(hwnd));
	SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)& device);
	UpdateWindow(hwnd);
	ShowWindow(hwnd, SW_SHOW);
	device.pushShader(GL_VERTEX_SHADER, (GLchar*)codeVertexShader, sizeof(codeVertexShader));
	device.pushShader(GL_FRAGMENT_SHADER, (GLchar*)codeFragmentShader, sizeof(codeFragmentShader));
	device.pushProgram();

	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			GLulong currentFrame = clock();
			device.delta_time = currentFrame - device.last_frame;
			device.last_frame = currentFrame;
			device.Render();
			InvalidateRect(hwnd, NULL, FALSE);
		}
	}
	return msg.wParam;
}
GLfloat function(float x, float y) {
	return x * x + y * y;
}
float4* genPositions(size_t width, size_t height, GLfloat& max, GLfloat& min, GLfloat far_draw_x, GLfloat near_draw_x, GLfloat far_draw_y, GLfloat near_draw_y) {
	float4* position = (float4*)malloc(width * height * sizeof(float4));
	ptrdiff_t start_h = -ptrdiff_t(height) / 2;
	ptrdiff_t start_w = -ptrdiff_t(width) / 2;
	GLfloat step_x = (far_draw_x - near_draw_x) / width;
	GLfloat step_y = (far_draw_y - near_draw_y) / height;

	for (ptrdiff_t y = start_h, _y = 0; _y < height; y++, _y++) {
		for (ptrdiff_t x = start_w, _x = 0; _x < width; x++, _x++) {
			GLfloat f_x = x * step_x;
			GLfloat f_y = y * step_y;
			GLfloat f_z = function(x, y);
			max = max < f_z ? f_z : max;
			min = min < f_z ? min : f_z;
			position[_y * width + _x] = float4(f_x, f_y, f_z - 1.0f, 1.0f);
		}
	}
	return position;
}
float4* genColors(float4* positions, GLfloat max, GLfloat min, size_t length) {
	float4* color = (float4*)malloc(length * sizeof(float4));
	GLfloat step = 1.0f / (2 * (max - min));
	GLfloat radius = 1.0f;
	GLfloat normalize = (2.0f * 3.1416f * radius) / max;
	for (GLint i = 0; i < length; i++) {
		GLfloat position = (-min + positions[i].z) * step;
		position = position < 0.0f ? 0.0f : position;
		positions[i].z = position;
		color[i] = { position, position, position, 1.0f };
	}
	return color;
}
GLuint* genIndices(size_t width, size_t height) {
	GLuint* index = (GLuint*)malloc((width * height) * sizeof(GLuint));
	for (size_t i = 0; i < width * height; i++) {
		index[i] = i;
	}
	return index;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static FLOAT monitor_width;
	static FLOAT monitor_height;
	static OpenGL_device* data;
	HDC hDC;
	PAINTSTRUCT ps;
	switch (uMsg) {
	case WM_CREATE:
	{
		HMONITOR monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
		MONITORINFO info;
		info.cbSize = sizeof(MONITORINFO);
		GetMonitorInfo(monitor, &info);
		monitor_width = info.rcMonitor.right - info.rcMonitor.left;
		monitor_height = info.rcMonitor.bottom - info.rcMonitor.top;

		break;
	}
	case WM_COMMAND:
		switch (wParam) {
		case ID_BUTTON_1: {
			GLfloat max = -999999, min = MAXDWORD32;
			size_t number_objects = 1024 * 1024;
			float4* positions = genPositions(1024, 1024, max, min, -1.0f, 1.0f, -1.0f, 1.0f);
			float4* colors = genColors(positions, max, min, number_objects);
			GLuint* indices = genIndices(1024, 1024);
			data->pushBuffer(positions, number_objects, sizeof(float4), GL_VECTOR_BUFFER);
			data->pushBuffer(colors, number_objects, sizeof(float4), GL_COLOR_BUFFER);
			data->pushBuffer(indices, number_objects, sizeof(GLuint), GL_INDEX_BUFFER);
			free(colors);
			free(positions);
			free(indices);
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
		case ID_BUTTON_2: {
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
		}
	case WM_SHOWWINDOW: {
		data = (OpenGL_device*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		break;
	}
	case WM_SETCURSOR:
	{
		//RECT rect;
		////SetCursor(NULL);
		//GetWindowRect(hWnd, &rect);
		//ClipCursor(&rect);
		return TRUE;
	}
	case WM_INPUT:
	{
		UINT sizeData = 0;
		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &sizeData, sizeof(RAWINPUTHEADER));
		BYTE* lpb = (BYTE*)_malloca(sizeData);
		RAWINPUT* raw = (RAWINPUT*)lpb;
		if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, raw, &sizeData, sizeof(RAWINPUTHEADER)) == sizeData)
			switch (raw->header.dwType)
			{
			case RIM_TYPEMOUSE:
			{
				data->new_mouse_position.x = data->old_mouse_position.x + raw->data.mouse.lLastX;
				data->new_mouse_position.y = data->old_mouse_position.y + raw->data.mouse.lLastY;
				FLOAT sensitivity = 1;
				FLOAT xoffset = FLOAT(data->new_mouse_position.x - data->old_mouse_position.x);
				FLOAT yoffset = FLOAT(data->old_mouse_position.y - data->new_mouse_position.y);
				data->old_mouse_position.x += data->new_mouse_position.x;
				data->old_mouse_position.y += data->new_mouse_position.y;

				xoffset *= sensitivity;
				yoffset *= sensitivity;
				data->y_angle += xoffset;
				data->x_angle += yoffset;

				if (data->x_angle > 89.0f)
					data->x_angle = 89.0f;
				if (data->x_angle < -89.0f)
					data->x_angle = -89.0f;

				data->getCenterView()->x = cos(M_PI * data->x_angle / 180.0f) * cos(M_PI * data->y_angle / 180.0f);
				data->getCenterView()->y = sin(M_PI * data->x_angle / 180.0f);
				data->getCenterView()->z = cos(M_PI * data->x_angle / 180.0f) * sin(M_PI * data->y_angle / 180.0f);
				*data->getCenterView() = normalize_vec3(*data->getCenterView());
				data->cameraRotate();

				break;
			}
			case RIM_TYPEKEYBOARD:
			{
				FLOAT cameraSpeed = 0.005f * data->delta_time;
				switch (raw->data.keyboard.VKey) {
				case VK_LEFT:
					*data->getEyeView() = *data->getEyeView() + (normalize_vec3(cross_vec3(*data->getCenterView(), *data->getUpView())) * cameraSpeed);
					data->cameraRotate();
					break;
				case VK_RIGHT: {
					*data->getEyeView() = *data->getEyeView() - (normalize_vec3(cross_vec3(*data->getCenterView(), *data->getUpView())) * cameraSpeed);
					data->cameraRotate();
					break;
				}
				case VK_UP: {
					*data->getEyeView() = *data->getEyeView() + (*data->getCenterView() * cameraSpeed);
					data->cameraRotate();
					break;
				}
				case VK_DOWN: {
					*data->getEyeView() = *data->getEyeView() - (*data->getCenterView() * cameraSpeed);
					data->cameraRotate();
					break;
				}
				}
			}
			}

		_freea(lpb);
		break;
	}
	//case WM_MOUSEMOVE: {
	//	data->new_mouse_position.x = LOWORD(lParam);
	//	data->new_mouse_position.y = HIWORD(lParam);
	//	break;
	//}
	case WM_KEYDOWN:
	{
		break;
	}
	case WM_SIZING: {
	}
	case WM_SIZE: {
		RECT rect;
		GetWindowRect(hWnd, &rect);
		glViewport(0, 0, rect.right - rect.left, rect.bottom - rect.top);
		if (data)
			data->genProjection(rect.right - rect.left, rect.bottom - rect.top, 1.0f, -1.0f, 45.0f);
		InvalidateRect(hWnd, 0, TRUE);
		break;
	}
	case WM_PAINT: {
		hDC = BeginPaint(hWnd, &ps);

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
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}