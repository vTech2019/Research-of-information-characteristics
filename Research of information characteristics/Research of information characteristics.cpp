#include <time.h>
#include <intrin.h>
#include "Figures.h"
#include "Math.h"
#include "LoadImage.h"
#include "OpenGL_device.h"
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

std::vector<dataImage> images;
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
	wc.lpszClassName = L"MainWindow";
	if (!RegisterClass(&wc))
		MessageBox(NULL, L"Failed to register window", L"Error!", MB_OK);
	hwnd = CreateWindow(L"MainWindow", L"OpenGL", WS_OVERLAPPEDWINDOW | WS_BORDER, NULL, NULL, 1280, 720, (HWND)NULL, NULL, HINSTANCE(Instance), NULL);

	RAWINPUTDEVICE Rid;
	Rid.usUsagePage = GENERIC;
	Rid.usUsage = Mouse;
	Rid.dwFlags = RIDEV_INPUTSINK;
	Rid.hwndTarget = hwnd;

	if (!RegisterRawInputDevices(&Rid, 1, sizeof(Rid))) {
		MessageBox(NULL, L"Raw devices registration failed!", L"Error!", MB_OK);
	}
	Rid.usUsage = Keyboard;
	if (!RegisterRawInputDevices(&Rid, 1, sizeof(Rid))) {
		MessageBox(NULL, L"Raw devices registration failed!", L"Error!", MB_OK);
	}
	HMENU hMenu = CreateMenu();
	HMENU menu_file = CreatePopupMenu();
	HMENU menu_graphics = CreatePopupMenu();
	AppendMenu(menu_graphics, MF_STRING | MF_POPUP, ID_BUTTON_1, L"Отрисовка функции");
	AppendMenu(menu_file, MF_STRING | MF_POPUP, ID_BUTTON_2, L"Чтение изображений");

	AppendMenu(menu_file, MF_STRING | MF_POPUP, ID_BUTTON_3, L"Сохранение изображений");
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)menu_file, L"Работа с файлами");
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)menu_graphics, L"OpenGL");
	SetMenu(hwnd, hMenu);

	OpenGL_device device(GetDC(hwnd));
	SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)& device);
	UpdateWindow(hwnd);
	ShowWindow(hwnd, SW_SHOW);
	device.pushShader(GL_VERTEX_SHADER, (GLchar*)codeVertexShader, sizeof(codeVertexShader));
	device.pushShader(GL_FRAGMENT_SHADER, (GLchar*)codeFragmentShader, sizeof(codeFragmentShader));
	device.pushProgram();

	LARGE_INTEGER qwWait, qwStart, qwCurrent;
	QueryPerformanceCounter(&qwStart);
	QueryPerformanceFrequency(&qwWait);
	qwWait.QuadPart >>= 5;
	unsigned __int64 Start = __rdtsc();
	do
	{
		QueryPerformanceCounter(&qwCurrent);
	} while (qwCurrent.QuadPart - qwStart.QuadPart < qwWait.QuadPart);
	double Hz = ((__rdtsc() - Start) << 5);
	size_t iHz = ((__rdtsc() - Start) << 5);
	size_t counter = 0;
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			size_t currentFrame = __rdtsc();
			device.delta_time = currentFrame - device.last_frame;
			device.last_frame = currentFrame;
			device.Render();
			InvalidateRect(hwnd, NULL, FALSE);
			counter++;
			if (device.delta_time < iHz)
				if (counter % (iHz / device.delta_time) == 0)
					SetWindowTextA(hwnd, std::to_string(Hz / device.delta_time).c_str());
		}
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//static FLOAT monitor_width;
	//static FLOAT monitor_height;
	static OpenGL_device* data;
	HDC hDC;
	PAINTSTRUCT ps;
	switch (uMsg) {
	case WM_CREATE:
	{
		//HMONITOR monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
		//MONITORINFO info;
		//info.cbSize = sizeof(MONITORINFO);
		//GetMonitorInfo(monitor, &info);
		//monitor_width = info.rcMonitor.right - info.rcMonitor.left;
		//monitor_height = info.rcMonitor.bottom - info.rcMonitor.top;

		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	case WM_COMMAND:
		switch (wParam) {
		case ID_BUTTON_1: {
			size_t number_images = images.size();
			size_t x_images = sqrt(number_images) + 1;
			size_t y_images = x_images;
			GLfloat step = 2.0f / x_images;
			for (size_t i = 0; i < y_images; i++) {
				for (size_t j = 0; j < x_images; j++) {
					if (i * x_images + j < number_images) {
						GLfloat start_x = GLfloat(j) * step - 1.0f;
						GLfloat start_y = GLfloat(i) * step - 1.0f;
						makeField(data, 2, 2, start_x + step, start_x, start_y + step, start_y);
					}
				}
			}
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
		case ID_BUTTON_2: {
			std::string fileName;
			fileName.resize(MAX_PATH * 256 * 1024);
			OPENFILENAME ofn = { 0 };
			//WCHAR fileName[MAX_PATH] = { 0 };
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = NULL;
			ofn.lpstrFile = (LPWSTR)fileName.data();
			ofn.nMaxFile = MAX_PATH * 256 * 1024;
			ofn.lpstrFilter = L"JPG\0*.jpg\0PNG\0*.png\0BMP\0*.bmp";
			ofn.nFilterIndex = 3;
			ofn.lpstrInitialDir = L"c:\\";
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT | OFN_EXPLORER;

			if (GetOpenFileName(&ofn)) {
				std::vector<std::vector<TCHAR>> names;
				std::vector<TCHAR> tmp;
				for (size_t i = ofn.nFileOffset; true; i++) {
					tmp.push_back(ofn.lpstrFile[i]);
					if (ofn.lpstrFile[i] == 0) {
						names.push_back(tmp);
						tmp.clear();
						if (ofn.lpstrFile[i + 1] == 0)
							break;
					}
				}
				if (!ofn.nFileExtension)
					load_images(ofn.lpstrFile, names, true, &images);
				else {
					load_image(ofn.lpstrFile, true, &images);
				}
				for (size_t i = 0; i < images.size(); i++)
					data->push2DTexture((GLubyte4*)images[i].get_data(), images[i].get_width(), images[i].get_height());
			}
			else {
			}
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
		case ID_BUTTON_3: {
			if (!images.empty())
				save_image(&images.back(), NULL, L"result2", JPG);
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

					data->getFrontView()->x = cos(M_PI * data->x_angle / 180.0f) * cos(M_PI * data->y_angle / 180.0f);
					data->getFrontView()->y = sin(M_PI * data->x_angle / 180.0f);
					data->getFrontView()->z = cos(M_PI * data->x_angle / 180.0f) * sin(M_PI * data->y_angle / 180.0f);
					*data->getFrontView() = normalize_vec3(*data->getFrontView());
					data->cameraRotate();
					break;
				}
			case RIM_TYPEKEYBOARD:
			{
				FLOAT cameraSpeed = 0.005f * data->delta_time;
				switch (raw->data.keyboard.VKey) {
				case VK_LEFT: {
					//*data->getEyeView() = *data->getEyeView() + (normalize_vec3(cross_vec3(*data->getFrontView(), *data->getUpView())) * 0.01);
					*data->getEyeView() = *data->getEyeView() + float4(0.1, 0.0, 0.0, 0.0);
					data->cameraRotate();
					break;
				}
				case VK_RIGHT: {
					*data->getEyeView() = *data->getEyeView() - (normalize_vec3(cross_vec3(*data->getFrontView(), *data->getUpView())) * 0.01);
					data->cameraRotate();
					break;
				}
				case VK_UP: {
					*data->getEyeView() = *data->getEyeView() + (*data->getFrontView() * 0.01);
					data->cameraRotate();
					break;
				}
				case VK_DOWN: {
					*data->getEyeView() = *data->getEyeView() - (*data->getFrontView() * 0.01);
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
		GetClientRect(hWnd, &rect);
		glViewport(0, 0, rect.right - rect.left, rect.bottom - rect.top);
		if (data)
			data->genProjection(rect.right - rect.left, rect.bottom - rect.top, 1.0f, -1.0f, 90.0f);
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