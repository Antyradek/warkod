#include <iostream>

#include "image.hpp"
#include "pixel.hpp"

int main(int argc, char** argv)
{
	if(argc != 2)
	{
		std::string programName(argv[0]);
		std::cerr << "Użycie: " << programName << " OBRAZ" << std::endl;
		return(-1);
	}
	
	std::string imageFilename = argv[1];
	
	std::cerr << "Przetwarzanie " << imageFilename << std::endl;
	
	warkod::Image<warkod::ColorfulPixel> baseImage(5, 5);
	std::cerr << "Rozmiar: " << baseImage.width() << " " << baseImage.height() << std::endl;
	
	warkod::Image<warkod::BinaryPixel> binaryImage(3,3);
	
	int pixelId = 0;
	for(warkod::ColorfulPixel pixel : baseImage)
	{
		pixel.red(0.5);
		std::cerr << "Piksel " << pixelId << ": " << pixel << std::endl;
		pixelId++;
	}
	
	pixelId = 0;
	for(warkod::BinaryPixel pixel : binaryImage)
	{
		std::cerr << "Piksel " << pixelId << ": " << pixel << std::endl;
		if(pixel.imagePositionX() + 1 < binaryImage.width())
		{
			std::cerr << "Sąsiad prawy: " << pixel.neighbour(1, 0) << std::endl;
		}
		pixelId++;
	}
	
	return(0);
}
