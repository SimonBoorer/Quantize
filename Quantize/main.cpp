#include <fstream>
#include "Quantizer.h"

int main()
{
	std::fstream file("test.raw", std::fstream::in | std::fstream::binary);

	file.seekg(0, std::ios::end);
	unsigned int image_size = (unsigned int) file.tellg();
	file.seekg(0, std::ios::beg);

	unsigned char* image = new unsigned char[image_size];
	file.read(reinterpret_cast<char*>(image), image_size);
	file.close();

	Quantizer quantizer(255, 8);
	quantizer.ProcessImage(image, image_size);

	file.close();
	file.open("palette.raw", std::fstream::out | std::fstream::binary);

	unsigned int colour_count = quantizer.GetColourCount();
	RGB* rgb = new RGB[colour_count]();
        quantizer.GetColourTable(rgb);

	for (unsigned int i = 0; i < colour_count; ++i)
		file << rgb[i].red << rgb[i].green << rgb[i].blue;

	file.close();
	file.open("quantize.raw", std::fstream::out | std::fstream::binary);

	for (unsigned int i = 0; i < image_size;)
	{
		unsigned char r = image[i++];
		unsigned char g = image[i++];
		unsigned char b = image[i++];

		unsigned int index = quantizer.GetColourIndex(r, g, b);
		file << rgb[index].red << rgb[index].green << rgb[index].blue;
	}

	file.close();

        delete[] rgb;
	delete[] image;
	return 0;
}

