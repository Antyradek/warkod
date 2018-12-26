#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "image.hpp"
#include "pixel.hpp"
#include "parameters.hpp"

int main(int argc, char** argv)
{
	if(argc != 2)
	{
		std::string programName(argv[0]);
		std::cerr << "Użycie: " << programName << " OBRAZ" << std::endl;
		return(-1);
	}
	
	std::string imageFilename = argv[1];
	std::string tmpDir = "/tmp/warkod/";
	
	//wczytanie obrazu
	std::cerr << "Wczytywanie " << imageFilename << "... " << std::flush;
	cv::Mat image = cv::imread(imageFilename);
	warkod::Image<warkod::ColorfulPixel> baseImage(image);
	cv::imwrite(tmpDir + "raw.jpg", baseImage.opencvImage());
	std::cerr << "gotowe " << std::endl;
	
	warkod::Parameters parameters;
	//oddzielenie czerwonego
	std::cerr << "Oddzielanie czerwonego... " << std::flush;
	warkod::Image<warkod::BinaryPixel> redImage(baseImage.width(),baseImage.height());
	for(const warkod::ColorfulPixel& pixel : baseImage)
	{
		double radius = ((parameters.lightRedRadius - parameters.darkRedRadius)) * pixel.red() + parameters.darkRedRadius;
		if(pixel.blue() * pixel.blue() + pixel.green() * pixel.green() > radius * radius)
		{
			//nie mieści się w stożku
			redImage.at(pixel.positionX(), pixel.positionY()).value(false);
		}
		else if(pixel.red() < parameters.darkRedTreshold)
		{
			//ciemna część ucięta
			redImage.at(pixel.positionX(), pixel.positionY()).value(false);
		}
		else
		{
			redImage.at(pixel.positionX(), pixel.positionY()).value(true);
		}
	}
	cv::imwrite(tmpDir + "red.png", redImage.opencvImage());
	std::cerr << "gotowe" << std::endl;
	
	//oddzielenie niebieskiego
	std::cerr << "Oddzielanie niebieskiego... " << std::flush;
	warkod::Image<warkod::BinaryPixel> blueImage(baseImage.width(),baseImage.height());
	for(const warkod::ColorfulPixel& pixel : baseImage)
	{
		double radius = ((parameters.lightBlueRadius - parameters.darkBlueRadius)) * pixel.blue() + parameters.darkBlueRadius;
		if(pixel.red() * pixel.red() + pixel.green() * pixel.green() > radius * radius)
		{
			//nie mieści się w stożku
			blueImage.at(pixel.positionX(), pixel.positionY()).value(false);
		}
		else if(pixel.blue() < parameters.darkBlueTreshold)
		{
			//ciemna część ucięta
			blueImage.at(pixel.positionX(), pixel.positionY()).value(false);
		}
		else
		{
			blueImage.at(pixel.positionX(), pixel.positionY()).value(true);
		}
	}
	cv::imwrite(tmpDir + "blue.png", blueImage.opencvImage());
	std::cerr << "gotowe" << std::endl;
	
	
	
// 	int pixelId = 0;
// 	for(warkod::ColorfulPixel pixel : baseImage)
// 	{
// 		pixel.red(0.5);
// // 		std::cerr << "Piksel " << pixelId << ": " << pixel << std::endl;
// 		pixelId++;
// 	}
	
// 	pixelId = 0;
// 	for(warkod::BinaryPixel pixel : binaryImage)
// 	{
// 		std::cerr << "Piksel " << pixelId << ": " << pixel << std::endl;
// 		if(pixel.imagePositionX() + 1 < binaryImage.width())
// 		{
// 			std::cerr << "Sąsiad prawy: " << pixel.neighbour(1, 0) << std::endl;
// 		}
// 		pixelId++;
// 	}
	
	return(0);
}
