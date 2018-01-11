#ifndef PALETTE_H_
#define PALETTE_H_

#include <cstdint>
#include <vector>

struct Colour
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
};

typedef std::vector<Colour> Palette;

std::size_t GetNearestColourIndex(const Palette& palette, const Colour& colour);

#endif // PALETTE_H_

