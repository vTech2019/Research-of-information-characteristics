#include "LoadImage.h"

VOID load_image(const TCHAR* name, BOOL RGBA, std::vector<dataImage>* image) {
	image->emplace_back();
	dataImage* local_image = &image->back();
	Gdiplus::GdiplusStartupInput input;
	Gdiplus::GdiplusStartupOutput output;
	ULONG_PTR token;
	Gdiplus::GdiplusStartup(&token, &input, &output);
	Gdiplus::Color color;
	Gdiplus::Bitmap* bitmap = new Gdiplus::Bitmap(name);
	Gdiplus::BitmapData bitmapData = {};
	Gdiplus::Rect rect(0, 0, bitmap->GetWidth(), bitmap->GetHeight());
	bitmap->LockBits(&rect, Gdiplus::ImageLockModeRead | Gdiplus::ImageLockModeWrite, PixelFormat24bppRGB, &bitmapData);

	UCHAR* pixels = (UCHAR*)bitmapData.Scan0;
	size_t height = bitmap->GetHeight();
	size_t stride = abs(bitmapData.Stride);
	local_image->set_height(height);
	local_image->set_width(bitmap->GetWidth());
	local_image->set_rgba(true);
	local_image->set_stride(stride);
	local_image->set_pixel_format(PixelFormat24bppRGB);
	local_image->allocate_memory(stride * height);
	UCHAR* write_pixels = (UCHAR*)local_image->get_data();
	for (size_t i = 0; i < height * stride; ++i)
		write_pixels[i] = pixels[i];
	bitmap->UnlockBits(&bitmapData);
	delete bitmap;
	Gdiplus::GdiplusShutdown(token);
}
VOID load_images(const TCHAR * directory, const std::vector<std::vector<TCHAR>> & files, BOOL RGBA, std::vector<dataImage>* image) {
	size_t length_directory = 0;
	image->reserve(image->size() + files.size());
	for (size_t i = 0; directory[i] != 0; i++)
		length_directory++;

	for (size_t i = 0; i < files.size(); i++) {
		std::vector<TCHAR> way(length_directory);
		memcpy(way.data(), directory, length_directory * sizeof(TCHAR));
		way.push_back(92);
		for (size_t j = 0; files[i][j] != 0; j++)
			way.push_back(files[i][j]);
		way.push_back(0);
		load_image(way.data(), RGBA, image);
	}
}
void save_image(dataImage* image, const TCHAR * directory_name, const TCHAR * name, size_t format) {
	Gdiplus::GdiplusStartupInput input;
	Gdiplus::GdiplusStartupOutput output;
	ULONG_PTR token;
	Gdiplus::GdiplusStartup(&token, &input, &output);
	Gdiplus::Color color;
	Gdiplus::Bitmap* bitmap = new Gdiplus::Bitmap(image->get_width(), image->get_height(), image->get_stride(), image->get_pixel_format(), (BYTE*)image->get_data());
	size_t length_dir = 0;
	size_t length_file_name = 0;
	CLSID clsid;
	const TCHAR* _name = name;
	if (directory_name)
		for (length_dir; directory_name[length_dir] != 0; length_dir++)
			length_dir++;
	for (length_file_name; name[length_file_name] != 0; length_file_name++)
		length_file_name++;
	TCHAR * name_file = (TCHAR*)_malloca((length_file_name + length_dir + 6) * sizeof(TCHAR));
	for (size_t i = 0; i < length_dir; i++)
		name_file[i] = directory_name[i];
	for (size_t i = length_dir; i < length_dir + length_file_name; i++)
		name_file[i] = name[i - length_dir];
	size_t length = length_dir + length_file_name;
	Gdiplus::Status result;
	switch (format) {
	case BMP:
		name_file[length] = L'.';
		name_file[length + 1] = L'b';
		name_file[length + 2] = L'm';
		name_file[length + 3] = L'p';
		name_file[length + 4] = 0;
		CLSIDFromString(_BMP, &clsid);
		result = bitmap->Save(name_file, &clsid, NULL);
		break;
	case JPG:
		name_file[length] = L'.';
		name_file[length + 1] = L'j';
		name_file[length + 2] = L'p';
		name_file[length + 3] = L'g';
		name_file[length + 4] = 0;
		CLSIDFromString(_JPG, &clsid);
		result = bitmap->Save(name_file, &clsid, NULL);
		break;
	case PNG:
		name_file[length] = L'.';
		name_file[length + 1] = L'p';
		name_file[length + 2] = L'n';
		name_file[length + 3] = L'g';
		name_file[length + 4] = 0;
		CLSIDFromString(_PNG, &clsid);
		result = bitmap->Save(name_file, &clsid, NULL);
		break;
	case TIF:
		name_file[length] = L'.';
		name_file[length + 1] = L't';
		name_file[length + 2] = L'i';
		name_file[length + 3] = L'f';
		name_file[length + 4] = L'f';
		name_file[length + 5] = 0;
		CLSIDFromString(_TIF, &clsid);
		result = bitmap->Save(name_file, &clsid, NULL);
		break;
	case GIF:
		name_file[length] = L'.';
		name_file[length + 1] = L'g';
		name_file[length + 2] = L'i';
		name_file[length + 3] = L'f';
		name_file[length + 4] = 0;
		CLSIDFromString(_GIF, &clsid);
		result = bitmap->Save(name_file, &clsid, NULL);
		break;
	}
	if (result != NOERROR);
	switch (result) {
	case CO_E_CLASSSTRING: {
		TCHAR info[] = L"The class string was improperly formatted.";
		MessageBox(NULL, info, L"Error!", MB_OK);
		break;
	}
	case REGDB_E_CLASSNOTREG: {
		TCHAR info[] = L"The CLSID corresponding to the class string was not found in the registry. ";
		MessageBox(NULL, info, L"Error!", MB_OK);
		break;
	}
	case	REGDB_E_READREGDB: {
		TCHAR info[] = L" The registry could not be opened for reading. ";
		MessageBox(NULL, info, L"Error!", MB_OK);
		break;
	}
	case 7: {
		TCHAR info[] = L" Win32Error ";
		MessageBox(NULL, info, L"Error!", MB_OK);
		break;
	}
	}
	_freea(name_file);
	delete bitmap;
	Gdiplus::GdiplusShutdown(token);
}