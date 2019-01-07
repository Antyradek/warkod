#include "dilation_filter.hpp" 

warkod::BinaryPixel warkod::DilationFilter::operator()(const warkod::BinaryPixel& pixel) const
{
	warkod::BinaryPixel newPixel(pixel);
	for(int diffX = -1; diffX <= 1; diffX++)
	{
		for(int diffY = -1; diffY <= 1; diffY++)
		{
			if(pixel.hasNeighbour(diffX, diffY) && pixel.neighbour(diffX, diffY).value())
			{
				newPixel.value(true);
				return(newPixel);
			}
		}
	}

	newPixel.value(false);
	return(newPixel);
}

