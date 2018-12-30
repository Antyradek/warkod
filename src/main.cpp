#include <iostream>
#include <sstream>
#include <iomanip>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "pixel.hpp"
#include "median_filter.hpp"
#include "image.hpp"
#include "parameters.hpp"

int main(int argc, char** argv)
{
	if(argc != 2 && argc != 3)
	{
		std::string programName(argv[0]);
		std::cerr << "Użycie: " << programName << " OBRAZ [KATALOG_WYGENEROWANYCH]" << std::endl;
		return(-1);
	}
	
	std::string imageFilename = argv[1];
	std::string tmpDir = "/tmp/warkod/";
	
	if(argc == 3)
	{
		tmpDir = argv[2];
		tmpDir += "/";
	}
	
	warkod::Parameters parameters;
	
	//wczytanie obrazu
	std::cerr << "Wczytywanie " << imageFilename << "... " << std::endl;
	cv::Mat image = cv::imread(imageFilename);
	warkod::Image<warkod::ColorfulPixel> baseImage(image);
	cv::imwrite(tmpDir + "raw.jpg", baseImage.opencvImage());
	
	//zastosowanie filtra medianowego
	std::cerr << "Filtr medianowy..." << std::endl;
	warkod::MedianFilter medianFilter(parameters.medianFilterRadius);
	baseImage.applyFilter(medianFilter);
	cv::imwrite(tmpDir + "median.jpg", baseImage.opencvImage());

	//oddzielenie czerwonego
	std::cerr << "Oddzielanie czerwonego... " << std::endl;
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
	
	//oddzielenie niebieskiego
	std::cerr << "Oddzielanie niebieskiego... " << std::endl;
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
	
	//wyciąganie obiektów z czerwonego obrazu
	std::cerr << "Wyciąganie obrazów z czerwonego..." << std::endl;
	bool found = true;
	int objectCounter = 1;
	while(found)
	{
		warkod::Image<warkod::BinaryPixel> redObject = redImage.findObject(found);
		if(found)
		{
			std::stringstream ss;
			ss << tmpDir << "red_obj_" << std::setw(3) << std::setfill('0') << objectCounter << ".png";
			std::cerr << "Wyciągnięto obiekt " << objectCounter << std::endl;
			warkod::InvariantMoments moments = redObject.calculateInvariantMoments();
			std::cout << ss.str() << " M1 " << moments.M1 << std::endl;
			std::cout << ss.str() << " M2 " << moments.M2 << std::endl;
			std::cout << ss.str() << " M3 " << moments.M3 << std::endl;
			cv::imwrite(ss.str(), redObject.opencvImage());
			objectCounter++;
		}
	}
	return(0);
}
