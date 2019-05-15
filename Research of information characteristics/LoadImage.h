#pragma once

#pragma once
#ifdef _WIN32
#include <Windows.h>
#include <gdiplus.h>
#include <Commdlg.h>
#include <immintrin.h>
#pragma comment(lib, "gdiplus.lib")
#endif
#include <vector>
class dataImage {
	size_t width;
	size_t height;
	size_t stride;
	INT pixelFormat;
	BOOL rgba;
	UCHAR* data;
	size_t size;
public:
	void set_width(size_t width) {
		this->width = width;
	}
	void set_height(size_t height) {
		this->height = height;
	}
	void set_stride(size_t stride) {
		this->stride = stride;
	}
	void set_pixel_format(INT pixelFormat) {
		this->pixelFormat = pixelFormat;
	}
	void set_rgba(bool rgba) {
		this->rgba = rgba;
	}
	size_t get_width() {
		return width;
	}
	size_t get_height() {
		return height;
	}
	size_t get_stride() {
		return stride;
	}
	INT get_pixel_format() {
		return pixelFormat;
	}
	BOOL get_rgba() {
		return rgba;
	}
	UCHAR& operator [](size_t i) {
		return  data[i];
	}
	
	UCHAR* get_data() {
		return data;
	}
	void allocate_memory(size_t size) {
		this->size = size;
		data = (UCHAR*)_mm_malloc(size, 32);
	}
	dataImage() {
		memset(this, 0, sizeof(*this));
	}
	~dataImage() {
		if (data)
			_mm_free(data);
	}
};
enum FORMATS { BMP, JPG, GIF, TIF, PNG };

#define _BMP L"{557cf400-1a04-11d3-9a73-0000f81ef32e}"
#define _JPG L"{557cf401-1a04-11d3-9a73-0000f81ef32e}"
#define _GIF L"{557cf402-1a04-11d3-9a73-0000f81ef32e}"
#define _TIF L"{557cf405-1a04-11d3-9a73-0000f81ef32e}"
#define _PNG L"{557cf406-1a04-11d3-9a73-0000f81ef32e}"
void save_image(dataImage& image, const TCHAR* directory_name, const TCHAR* name, size_t format);
dataImage& load_image(const TCHAR* name, BOOL RGBA);
std::vector< dataImage>& load_images(const TCHAR* directory,const std::vector<std::vector<TCHAR>>& files, BOOL RGBA);
