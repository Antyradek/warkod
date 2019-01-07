#include "erosion_filter.hpp" 

warkod::BinaryPixel warkod::ErosionFilter::operator()(const warkod::BinaryPixel& pixel) const
{
	warkod::BinaryPixel newPixel(pixel);
	
	for(int diffY = -1; diffY <= 1; diffY++)
	{
		for(int diffX = -1; diffX <= 1; diffX++)
		{
			if(!pixel.hasNeighbour(diffX, diffY) || !pixel.neighbour(diffX, diffY).value())
			{
				newPixel.value(false);
				return(newPixel);
			}
		}
	}
	
	newPixel.value(true);
	return(newPixel);
}

