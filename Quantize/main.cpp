#include <boost/program_options.hpp>
using namespace boost::program_options;

#include <iostream>
using namespace std;

#include <OpenImageIO/imageio.h>
OIIO_NAMESPACE_USING

#include "Quantizer.h"

int main(int argc, char* argv[])
{
	try
	{
		options_description desc("Allowed options");
		desc.add_options()
			("help,h", "produce help message")
			("max-colours,m", value<unsigned int>()->default_value(255),
				"maximum number of colours in the palette")
			("colour-bits,b", value<unsigned int>()->default_value(8),
				"number of significant bits in each 8-bit colour compartment")
		;

		options_description hidden("Hidden options");
		hidden.add_options()
			("input-file", value<string>()->required(), "input file")
			("output-file", value<string>()->required(), "output file")
		;

		options_description all("Allowed options");
		all.add(desc).add(hidden);

		positional_options_description p;
		p.add("input-file", 1);
		p.add("output-file", 1);

		variables_map vm;
		store(command_line_parser(argc, argv).options(all)
			.positional(p).run(), vm);

		if (vm.count("help"))
		{
			cout << "Usage: " << argv[0] << " [options] input-file output-file" << endl;
			cout << desc << endl;
			return 0;
		}

		notify(vm);

		string inputfile = vm["input-file"].as<string>();
		ImageInput* in = ImageInput::open(inputfile);
		if (!in)
		{
			cerr << "Could not create an ImageInput for "
				<< inputfile << ", error = "
				<< OpenImageIO::geterror() << endl;
			return 0;
		}

		const ImageSpec& inspec = in->spec();
		int xres = inspec.width;
		int yres = inspec.height;
		int channels = inspec.nchannels;
		int image_bytes = xres * yres * channels;
		vector<unsigned char> inpixels(image_bytes);

		if (!in->read_image(TypeDesc::UINT8, &inpixels[0]))
		{
			cerr << "Could not read pixels from " << inputfile
				<< ", error = " << in->geterror() << endl;
			ImageInput::destroy(in);
			return 0;
		}

		unsigned int max_colours = vm["max-colours"].as<unsigned int>();
		unsigned int colour_bits = vm["colour-bits"].as<unsigned int>();

		Quantizer quantizer(max_colours, colour_bits);
		quantizer.ProcessImage(&inpixels[0], image_bytes);

		string outputfile = vm["output-file"].as<string>();
		ImageOutput* out = ImageOutput::create(outputfile);
		if (!out)
		{
			cerr << "Could not create an ImageOutput for "
				<< outputfile << ", error = "
				<< OpenImageIO::geterror() << endl;
			ImageInput::destroy(in);
			return 0;
		}

		ImageSpec outspec = inspec;
		if (!out->open(outputfile, outspec))
		{
			cerr << "Could not open " << outputfile
				<< ", error = " << out->geterror() << endl;
			ImageInput::destroy(in);
			ImageOutput::destroy(out);
			return 0;
		}

		unsigned int colour_count = quantizer.GetColourCount();
		vector<RGB> rgb(colour_count);
		quantizer.GetColourTable(&rgb[0]);

		vector<unsigned char> outpixels(image_bytes);

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
			cerr << "Could not write pixels to " << outputfile
				<< ", error = " << out->geterror() << endl;
			ImageInput::destroy(in);
			ImageOutput::destroy(out);
			return 0;
		}

		if (!in->close())
		{
			cerr << "Error closing " << inputfile
				<< ", error = " << in->geterror() << endl;
			ImageInput::destroy(in);
			ImageOutput::destroy(out);
			return 0;
		}

		ImageInput::destroy(in);

		if (!out->close())
		{
			cerr << "Error closing " << outputfile
				<< ", error = " << out->geterror() << endl;
			ImageOutput::destroy(out);
			return 0;
		}

		ImageOutput::destroy(out);
	}
	catch (exception& e)
	{
		cerr << e.what() << endl;
	}
}

