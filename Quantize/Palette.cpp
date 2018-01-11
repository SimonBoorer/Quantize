#include "Palette.h"

#include <climits>

std::size_t GetNearestColourIndex(const Palette& palette, const Colour& colour)
{
	int min_distance = INT_MAX;
	std::size_t min_index = 0;

	for (auto it = palette.begin(); it != palette.end(); ++it)
	{
		int red_distance = it->r - colour.r;
		int green_distance = it->g - colour.g;
		int blue_distance = it->b - colour.b;

		int distance = (red_distance * red_distance) + 
			(green_distance * green_distance) + 
			(blue_distance * blue_distance);

		if (distance < min_distance)
		{
			min_distance = distance;
			min_index = it - palette.begin();
		}
	}

	return min_index;
}
