#include "LoadImage.h"

dataImage& load_image(const TCHAR* name, BOOL RGBA) {
	dataImage* image = (dataImage*)malloc(sizeof(dataImage));
	Gdiplus::GdiplusStartupInput input;
	Gdiplus::GdiplusStartupOutput output;
	ULONG_PTR token;
	Gdiplus::GdiplusStartup(&token, &input, &output);
	Gdiplus::Color color;
	Gdiplus::Bitmap* bitmap = new Gdiplus::Bitmap(name);
	Gdiplus::BitmapData bitmapData = {};
	Gdiplus::Rect rect(0, 0, bitmap->GetWidth(), bitmap->GetHeight());
	bitmap->LockBits(&rect, Gdiplus::ImageLockModeRead | Gdiplus::ImageLockModeWrite, PixelFormat32bppRGB, &bitmapData);

	UCHAR* pixels = (UCHAR*)bitmapData.Scan0;
	size_t height = bitmap->GetHeight();
	size_t stride = abs(bitmapData.Stride);
	image->set_height(height);
	image->set_width(bitmap->GetWidth());
	image->set_rgba(true);
	image->set_stride(stride);
	image->set_pixel_format(PixelFormat32bppRGB);
	image->allocate_memory(stride * height);
	for (size_t row = 0; row < height; ++row)
		for (size_t col = 0; col < stride; ++col)
		{
			image->operator[](row * stride + col) = pixels[row * stride + col];
		}
	bitmap->UnlockBits(&bitmapData);
	delete bitmap;
	Gdiplus::GdiplusShutdown(token);

	return *image;
}
std::vector< dataImage>& load_images(const TCHAR * directory, const std::vector<std::vector<TCHAR>> & files, BOOL RGBA) {
	std::vector< dataImage>* images = (std::vector< dataImage>*)malloc(sizeof(std::vector< dataImage>));
	images->resize(files.size());
	size_t length_directory = 0;
	for (size_t i = 0; directory[i] != 0; i++)
		length_directory++;

	for (size_t i = 0; i < files.size(); i++) {
		std::vector<TCHAR> way(length_directory);
		memcpy(way.data(), directory, length_directory * sizeof(TCHAR));
		way.push_back(92);
		for (size_t j = 0; files[i][j] != 0; j++)
			way.push_back(files[i][j]);
		way.push_back(0);
		(*images)[i] = load_image(way.data(), RGBA);
	}
	return *images;
}
void save_image(dataImage & image, const TCHAR * directory_name, const TCHAR * name, size_t format) {
	Gdiplus::GdiplusStartupInput input;
	Gdiplus::GdiplusStartupOutput output;
	ULONG_PTR token;
	Gdiplus::GdiplusStartup(&token, &input, &output);
	Gdiplus::Color color;
	Gdiplus::Bitmap* bitmap = new Gdiplus::Bitmap(image.get_width(), image.get_height(), image.get_stride(), image.get_pixel_format(), (BYTE*)image.get_data());
	size_t length_dir;
	size_t length_file_name;
	CLSID clsid;
	const TCHAR* _name = name;
	for (length_dir = 0; directory_name[length_dir] != 0; length_dir++)
		length_dir++;
	for (length_file_name = 0; name[length_file_name] != 0; length_file_name++)
		length_file_name++;
	TCHAR * name_file = (TCHAR*)_malloca((length_file_name + length_dir + 6) * sizeof(TCHAR));
	for (size_t i = 0; i < length_dir; i++)
		name_file[i] = directory_name[i];
	for (size_t i = length_dir; i < length_dir + length_file_name; i++)
		name_file[i] = name[i - length_dir];
	size_t length = length_dir + length_file_name;
	switch (format) {
	case BMP:
		name_file[length] = L'.';
		name_file[length + 1] = L'b';
		name_file[length + 2] = L'm';
		name_file[length + 3] = L'p';
		name_file[length + 4] = 0;
		CLSIDFromString(_BMP, &clsid);
		bitmap->Save(name_file, &clsid, NULL);
		break;
	case JPG:
		name_file[length] = L'.';
		name_file[length + 1] = L'j';
		name_file[length + 2] = L'p';
		name_file[length + 3] = L'g';
		name_file[length + 4] = 0;
		CLSIDFromString(_JPG, &clsid);
		bitmap->Save(name_file, &clsid, NULL);
		break;
	case PNG:
		name_file[length] = L'.';
		name_file[length + 1] = L'p';
		name_file[length + 2] = L'n';
		name_file[length + 3] = L'g';
		name_file[length + 4] = 0;
		CLSIDFromString(_PNG, &clsid);
		bitmap->Save(name_file, &clsid, NULL);
		break;
	case TIF:
		name_file[length] = L'.';
		name_file[length + 1] = L't';
		name_file[length + 2] = L'i';
		name_file[length + 3] = L'f';
		name_file[length + 4] = L'f';
		name_file[length + 5] = 0;
		CLSIDFromString(_TIF, &clsid);
		bitmap->Save(name_file, &clsid, NULL);
		break;
	case GIF:
		name_file[length] = L'.';
		name_file[length + 1] = L'g';
		name_file[length + 2] = L'i';
		name_file[length + 3] = L'f';
		name_file[length + 4] = 0;
		CLSIDFromString(_GIF, &clsid);
		bitmap->Save(name_file, &clsid, NULL);
		break;
	}
	_freea(name_file);
	delete bitmap;
	Gdiplus::GdiplusShutdown(token);
}