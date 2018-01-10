#include <boost/program_options.hpp>
using namespace boost::program_options;

#include <iostream>
using namespace std;

#include <OpenImageIO/imageio.h>
using namespace OIIO;

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
				<< OIIO::geterror() << endl;
			return 0;
		}

		unsigned int max_colours = vm["max-colours"].as<unsigned int>();
		unsigned int colour_bits = vm["colour-bits"].as<unsigned int>();
		Quantizer quantizer(max_colours, colour_bits);

		int num_of_subimages = 0;
		ImageSpec spec;
		while (in->seek_subimage(num_of_subimages, 0, spec))
		{
			// Note: spec has the format of the current subimage/miplevel
			int xres = spec.width;
			int yres = spec.height;
			int channels = spec.nchannels;
			int image_bytes = xres * yres * channels;
			vector<unsigned char> pixels(image_bytes);

			if (!in->read_image(TypeDesc::UINT8, &pixels[0]))
			{
				cerr << "Could not read pixels from " << inputfile
					<< ", error = " << in->geterror() << endl;
				ImageInput::destroy(in);
				return 0;
			}

			quantizer.ProcessImage(&pixels[0], image_bytes);

			++num_of_subimages;
		}

		string outputfile = vm["output-file"].as<string>();
		ImageOutput* out = ImageOutput::create(outputfile);
		if (!out)
		{
			cerr << "Could not create an ImageOutput for "
				<< outputfile << ", error = "
				<< OIIO::geterror() << endl;
			ImageInput::destroy(in);
			return 0;
		}

		// Be sure we can support subimages
		if (num_of_subimages > 1 && (!out->supports("multiimage") || !out->supports("appendsubimage")))
		{
			std::cerr << "Does not support appending of subimages" << std::endl;
			ImageInput::destroy(in);
			ImageOutput::destroy(out);
			return 0;
		}

		unsigned int colour_count = quantizer.GetColourCount();
		vector<RGB> rgb(colour_count);
		quantizer.GetColourTable(&rgb[0]);

		// Use Create mode for the first level.
		ImageOutput::OpenMode appendmode = ImageOutput::Create;

		for (int i = 0; i < num_of_subimages; ++i)
		{
			in->seek_subimage(i, 0, spec);

			if (!out->open(outputfile, spec, appendmode))
			{
				cerr << "Could not open " << outputfile
					<< ", error = " << out->geterror() << endl;
				ImageInput::destroy(in);
				ImageOutput::destroy(out);
				return 0;
			}

			// Note: spec has the format of the current subimage/miplevel
			int xres = spec.width;
			int yres = spec.height;
			int channels = spec.nchannels;
			int image_bytes = xres * yres * channels;
			vector<unsigned char> pixels(image_bytes);

			in->read_image(TypeDesc::UINT8, &pixels[0]);

			for (unsigned int j = 0; j < image_bytes; j += 3)
			{
				unsigned char r = pixels[j];
				unsigned char g = pixels[j + 1];
				unsigned char b = pixels[j + 2];

				unsigned int index = quantizer.GetColourIndex(r, g, b);

				pixels[j] = rgb[index].red;
				pixels[j + 1] = rgb[index].green;
				pixels[j + 2] = rgb[index].blue;
			}

			if (!out->write_image(TypeDesc::UINT8, &pixels[0]))
			{
				cerr << "Could not write pixels to " << outputfile
					<< ", error = " << out->geterror() << endl;
				ImageInput::destroy(in);
				ImageOutput::destroy(out);
				return 0;
			}

			// Use AppendSubimage mode for subsequent levels
			appendmode = ImageOutput::AppendSubimage;
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

