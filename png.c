#include <string.h>			// malloc, free
#include <stdlib.h>			// memset
#include <libpng.h>			// libpng library

#pragma comment(lib, "libpng.lib")

unsigned char* read_png(const char* filename, int* width, int* height, int* channel)
{
	png_image image;

	memset(&image, 0, sizeof(image));
	image.version = PNG_IMAGE_VERSION;

	if (png_image_begin_read_from_file(&image, filename))
	{
		png_bytep buffer;
		png_int_32 row_stride;

		image.format = PNG_FORMAT_RGBA;
		buffer = malloc(PNG_IMAGE_SIZE(image));
		row_stride = -1 * PNG_IMAGE_ROW_STRIDE(image);

		if (png_image_finish_read(&image, NULL, buffer, row_stride, NULL))
		{
			*width = image.width;
			*height = image.height;
			*channel = PNG_IMAGE_PIXEL_CHANNELS(image.format);
			return (unsigned char*)buffer;
		}
		else
		{
			free(buffer);
			png_image_free(&image);
		}
	}
	return 0;
};

void free_png(unsigned char* data)
{
	free(data);
}