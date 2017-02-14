#include <iostream>
#include <OpenImageIO/imageio.h>
OIIO_NAMESPACE_USING

#include "Quantizer.h"

int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		std::cout << "Usage: quantizer inputfile outputfile" << std::endl;
		return 0;
	}

	char* inputfile = argv[1];
	ImageInput* in = ImageInput::open(inputfile);
	if (!in)
	{
		std::cerr << "Could not create an ImageInput for "
			<< inputfile << ", error = "
			<< OpenImageIO::geterror() << std::endl;
		return 0;
	}

	const ImageSpec& inspec = in->spec();
	int xres = inspec.width;
	int yres = inspec.height;
	int channels = inspec.nchannels;
	int image_bytes = xres * yres * channels;
	std::vector<unsigned char> inpixels(image_bytes);

	if (!in->read_image(TypeDesc::UINT8, &inpixels[0]))
	{
		std::cerr << "Could not read pixels from " << inputfile
			<< ", error = " << in->geterror() << std::endl;
		ImageInput::destroy(in);
		return 0;
	}

	Quantizer quantizer(255, 8);
	quantizer.ProcessImage(&inpixels[0], image_bytes);

	char* outputfile = argv[2];
	ImageOutput* out = ImageOutput::create(outputfile);
	if (!out)
	{
		std::cerr << "Could not create an ImageOutput for "
			<< outputfile << ", error = "
			<< OpenImageIO::geterror() << std::endl;
		ImageInput::destroy(in);
		return 0;
	}

	ImageSpec outspec = inspec;
	if (!out->open(outputfile, outspec))
	{
		std::cerr << "Could not open " << outputfile
			<< ", error = " << out->geterror() << std::endl;
		ImageInput::destroy(in);
		ImageOutput::destroy(out);
		return 0;
	}

	unsigned int colour_count = quantizer.GetColourCount();
	std::vector<RGB> rgb(colour_count);
	quantizer.GetColourTable(&rgb[0]);

	std::vector<unsigned char> outpixels(image_bytes);

	for (unsigned int i = 0; i < image_bytes; i += 3)
	{
		unsigned char r = inpixels[i];
		unsigned char g = inpixels[i + 1];
		unsigned char b = inpixels[i + 2];

		unsigned int index = quantizer.GetColourIndex(r, g, b);

		outpixels[i] = rgb[index].red;
		outpixels[i + 1] = rgb[index].green;
		outpixels[i + 2] = rgb[index].blue;
	}

	if (!out->write_image(TypeDesc::UINT8, &outpixels[0]))
	{
		std::cerr << "Could not write pixels to " << outputfile
			<< ", error = " << out->geterror() << std::endl;
		ImageInput::destroy(in);
		ImageOutput::destroy(out);
		return 0;
	}

	if (!in->close())
	{
		std::cerr << "Error closing " << inputfile
			<< ", error = " << in->geterror() << std::endl;
		ImageInput::destroy(in);
		ImageOutput::destroy(out);
		return 0;
	}

	ImageInput::destroy(in);

	if (!out->close())
	{
		std::cerr << "Error closing " << outputfile
			<< ", error = " << out->geterror() << std::endl;
		ImageOutput::destroy(out);
		return 0;
	}

	ImageOutput::destroy(out);

	return 0;
}

