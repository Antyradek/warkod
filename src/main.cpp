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
	std::cerr << "Piksel w: " << baseImage.at(2, 3).red() << std::endl;
	
	for(warkod::ColorfulPixel pixel : baseImage)
	{
		std::cerr << "Piksel kolejny: " << pixel.red() << std::endl;
	}
	
	return(0);
}
