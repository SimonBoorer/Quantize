#ifndef DITHER_H_
#define DITHER_H_

#include <cstdint>

#include "Palette.h"

void DitherImage(uint8_t* pixels, int width, int height, Palette palette);

#endif // DITHER_H_

