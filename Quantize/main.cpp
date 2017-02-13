#include <fstream>
#include "Quantizer.h"

int main(int argc, char *argv[])
{
	std::fstream file("test.raw", std::fstream::in | std::fstream::binary);

	file.seekg(0, std::ios::end);
	unsigned int nImageSize = (unsigned int) file.tellg();
	file.seekg(0, std::ios::beg);

	unsigned char *pImage = new unsigned char[nImageSize];
	file.read((char *)pImage, nImageSize);
	file.close();

	Quantize cQuantizer(255, 8);
	cQuantizer.ProcessImage(pImage, nImageSize);

	file.close();
	file.open("palette.raw", std::fstream::out | std::fstream::binary);

	RGB *prgb = cQuantizer.GetColourTable();
	unsigned int nColourCount = cQuantizer.GetColourCount();

	for(unsigned int i = 0; i < nColourCount; ++i)
		file << prgb[i].red << prgb[i].green << prgb[i].blue;

	file.close();
	file.open("quantize.raw", std::fstream::out | std::fstream::binary);

	for(unsigned int i = 0; i < nImageSize;)
	{
		unsigned char r = pImage[i++];
		unsigned char g = pImage[i++];
		unsigned char b = pImage[i++];

		unsigned int nColourIndex = cQuantizer.GetColourIndex(r, g, b);
		file << prgb[nColourIndex].red << prgb[nColourIndex].green << prgb[nColourIndex].blue;
	}

	file.close();

	delete[] pImage;
	return 0;
}
