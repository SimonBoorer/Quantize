#include "Dither.h"

void DitherImage(uint8_t* pixels, int width, int height, Palette palette)
{
	int num_pixels = width * height;

    for(int y = 0; y < height; ++y)
    {
        for(int x = 0; x < height; ++x)
        {
            uint8_t* pixel = pixels + 3 * (y * width + x);

            // Compute the colors we want (rounding to nearest)
			Colour old_pixel = { pixel[0], pixel[1], pixel[2] };
            
            // Search the palete
			Colour new_pixel = palette[GetNearestColourIndex(palette, old_pixel)];
            
            // Write the result to the temp buffer
            uint8_t r_error = old_pixel.r - new_pixel.r;
            uint8_t g_error = old_pixel.g - new_pixel.g;
            uint8_t b_error = old_pixel.b - new_pixel.b;
            
            pixel[0] = new_pixel.r;
            pixel[1] = new_pixel.g;
            pixel[2] = new_pixel.b;

            // Propagate the eror to the four adjacent locations
            // that we haven't touched yet
            int quantloc_7 = (y * width + x + 1);
            int quantloc_3 = (y * width + width + x - 1);
            int quantloc_5 = (y * width + width + x);
            int quantloc_1 = (y * width + width + x + 1);
            
            if(quantloc_7 < num_pixels)
            {
                uint8_t* pixel7 = pixels + 3 * quantloc_7;
                pixel7[0] += r_error * 7 / 16;
                pixel7[1] += g_error * 7 / 16;
                pixel7[2] += b_error * 7 / 16;
            }
            
            if(quantloc_3 < num_pixels)
            {
                uint8_t* pixel3 = pixels + 3 * quantloc_3;
                pixel3[0] += r_error * 3 / 16;
                pixel3[1] += g_error * 3 / 16;
                pixel3[2] += b_error * 3 / 16;
            }
            
            if(quantloc_5 < num_pixels)
            {
                uint8_t* pixel5 = pixels + 3 * quantloc_5;
                pixel5[0] += r_error * 5 / 16;
                pixel5[1] += g_error * 5 / 16;
                pixel5[2] += b_error * 5 / 16;
            }
            
            if(quantloc_1 < num_pixels)
            {
                uint8_t* pixel1 = pixels + 3 * quantloc_1;
                pixel1[0] += r_error / 16;
                pixel1[1] += g_error / 16;
                pixel1[2] += b_error / 16;
            }
        }
    }
}

