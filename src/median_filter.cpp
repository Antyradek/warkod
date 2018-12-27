#include "median_filter.hpp" 
#include <vector>
#include <algorithm>
#include <iostream>

warkod::MedianFilter::MedianFilter(int squareRadius) : radius(squareRadius)
{
}


warkod::ColorfulPixel warkod::MedianFilter::operator()(const warkod::ColorfulPixel& pixel) const
{
	std::vector<double> redPixels;
	std::vector<double> greenPixels;
	std::vector<double> bluePixels;
	//zarezerwuj trochę przestrzeni dla optymalizacji
	const int squareBorder = 2 * radius + 1;
	redPixels.reserve(squareBorder * squareBorder);
	greenPixels.reserve(squareBorder * squareBorder);
	bluePixels.reserve(squareBorder * squareBorder);
	
	//umieść wszystkich sąsiadów w buforze
	for(int skewY = -radius; skewY <= radius; skewY++)
	{
		for(int skewX = -radius; skewX <= radius; skewX++)
		{
			if(pixel.hasNeighbour(skewX, skewY))
			{
				redPixels.push_back(pixel.neighbour(skewX, skewY).red());
				greenPixels.push_back(pixel.neighbour(skewX, skewY).green());
				bluePixels.push_back(pixel.neighbour(skewX, skewY).blue());
			}
		}
	}
	//posortuj bufory
	std::sort(redPixels.begin(), redPixels.end());
	std::sort(greenPixels.begin(), greenPixels.end());
	std::sort(bluePixels.begin(), bluePixels.end());
	
	double medianRed = redPixels.at(redPixels.size() / 2);
	double medianGreen = greenPixels.at(greenPixels.size() / 2);
	double medianBlue = bluePixels.at(bluePixels.size() / 2);
	
	//stwórz i zwróć nowy piksel
	warkod::ColorfulPixel newPixel(pixel);
	newPixel.red(medianRed);
	newPixel.green(medianGreen);
	newPixel.blue(medianBlue);
	
	return(newPixel);
}
