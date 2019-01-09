#include <iostream>
#include <sstream>
#include <iomanip>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "pixel.hpp"
#include "median_filter.hpp"
#include "image.hpp"
#include "parameters.hpp"
#include "dilation_filter.hpp"
#include "erosion_filter.hpp"

/// Oblicz odległość euklidesową obiektu o określonych niezmiennikach od podanego wzoru
double calculateEuclidianDistance(const warkod::InvariantMoments& invariantMoments, const warkod::ObjectParameters& parameters)
{
	double invariants[10] = {invariantMoments.M1, invariantMoments.M2, invariantMoments.M3, invariantMoments.M4, invariantMoments.M5, invariantMoments.M6, invariantMoments.M7, invariantMoments.M8, invariantMoments.M9, invariantMoments.M10};
	
	double sum = 0;
	std::cerr << "Odległości: ";
	for(int i = 0; i < 10; i++)
	{
		double squaredDistance = std::pow(invariants[i] - parameters.invariants[i], 2) / std::pow(parameters.sigmas[i], 2);
		std::cerr << "M" << i + 1 << " " << std::sqrt(squaredDistance) << " ";
		sum += squaredDistance * parameters.weights[i];
	}
	return(std::sqrt(sum));
	std::cerr << std::endl;
}

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
	const warkod::MedianFilter medianFilter(parameters.medianFilterRadius);
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
	
	//otwieranie
	int openingDepth = parameters.openingWidth * std::min(baseImage.width(), baseImage.height());
	std::cerr << "Otwieranie z głębią " << openingDepth << "..." << std::endl;
	warkod::Image<warkod::ColorfulPixel> tempor(baseImage);

	for(int i = 0; i < openingDepth; i++)
	{
		const warkod::ErosionFilter filter;
		blueImage.applyFilter(filter);
		redImage.applyFilter(filter);
	}
	for(int i = 0; i < openingDepth; i++)
	{
		const warkod::DilationFilter filter;
		blueImage.applyFilter(filter);
		redImage.applyFilter(filter);
	}
	cv::imwrite(tmpDir + "blue_opened.png", blueImage.opencvImage());
	cv::imwrite(tmpDir + "red_opened.png", redImage.opencvImage());

	
	//wyciąganie obiektów z czerwonego obrazu
	std::cerr << "Wyciąganie obrazów z czerwonego..." << std::endl;
	bool found = true;
	int objectCounter = 1;
	double minRedArrowDistance = std::numeric_limits<double>().max();
	warkod::Image<warkod::BinaryPixel> bestRedArrowObject(baseImage.width(), baseImage.height());
	while(found)
	{
		warkod::Image<warkod::BinaryPixel> redObject = redImage.findObject(found);
		if(found)
		{
			std::stringstream ss;
			ss << tmpDir << "obj_" << std::setw(4) << std::setfill('0') << objectCounter << ".png";
			std::cerr << "Wyciągnięto obiekt " << objectCounter << std::endl;
			warkod::InvariantMoments moments = redObject.calculateInvariantMoments();
			std::cout << ss.str() << " M1 " << moments.M1 << std::endl;
			std::cout << ss.str() << " M2 " << moments.M2 << std::endl;
			std::cout << ss.str() << " M3 " << moments.M3 << std::endl;
			std::cout << ss.str() << " M4 " << moments.M4 << std::endl;
			std::cout << ss.str() << " M5 " << moments.M5 << std::endl;
			std::cout << ss.str() << " M6 " << moments.M6 << std::endl;
			std::cout << ss.str() << " M7 " << moments.M7 << std::endl;
			std::cout << ss.str() << " M8 " << moments.M8 << std::endl;
			std::cout << ss.str() << " M9 " << moments.M9 << std::endl;
			std::cout << ss.str() << " M10 " << moments.M10 << std::endl;
			cv::imwrite(ss.str(), redObject.opencvImage());
			objectCounter++;
			
			double redArrowDistance = calculateEuclidianDistance(moments, parameters.redArrowParams);
			std::cerr << "Odległość czerwonej strzałki: " << redArrowDistance << std::endl;
			if(redArrowDistance < minRedArrowDistance)
			{
				minRedArrowDistance = redArrowDistance;
				bestRedArrowObject = redObject;
			}
		}
	}
	cv::imwrite(tmpDir + "best_red_arrow.png", bestRedArrowObject.opencvImage());
	
	//wyciąganie obiektów z niebieskiego obrazu
	std::cerr << "Wyciąganie obrazów z niebieskiego..." << std::endl;
	found = true;
	double minBlueArrowDistance = std::numeric_limits<double>().max();
	double minLetterWDistance = std::numeric_limits<double>().max();
	double minLetterKDistance = std::numeric_limits<double>().max();
	double minLetterDDistance = std::numeric_limits<double>().max();
	warkod::Image<warkod::BinaryPixel> bestBlueArrowObject(baseImage.width(), baseImage.height());
	warkod::Image<warkod::BinaryPixel> bestLetterWObject(baseImage.width(), baseImage.height());
	warkod::Image<warkod::BinaryPixel> bestLetterKObject(baseImage.width(), baseImage.height());
	warkod::Image<warkod::BinaryPixel> bestLetterDObject(baseImage.width(), baseImage.height());
	while(found)
	{
		warkod::Image<warkod::BinaryPixel> blueObject = blueImage.findObject(found);
		if(found)
		{
			std::stringstream ss;
			ss << tmpDir << "obj_" << std::setw(4) << std::setfill('0') << objectCounter << ".png";
			std::cerr << "Wyciągnięto obiekt " << objectCounter << std::endl;
			warkod::InvariantMoments moments = blueObject.calculateInvariantMoments();
			std::cout << ss.str() << " M1 " << moments.M1 << std::endl;
			std::cout << ss.str() << " M2 " << moments.M2 << std::endl;
			std::cout << ss.str() << " M3 " << moments.M3 << std::endl;
			std::cout << ss.str() << " M4 " << moments.M4 << std::endl;
			std::cout << ss.str() << " M5 " << moments.M5 << std::endl;
			std::cout << ss.str() << " M6 " << moments.M6 << std::endl;
			std::cout << ss.str() << " M7 " << moments.M7 << std::endl;
			std::cout << ss.str() << " M8 " << moments.M8 << std::endl;
			std::cout << ss.str() << " M9 " << moments.M9 << std::endl;
			std::cout << ss.str() << " M10 " << moments.M10 << std::endl;
			cv::imwrite(ss.str(), blueObject.opencvImage());
			objectCounter++;
			
			double blueArrowDistance = calculateEuclidianDistance(moments, parameters.blueArrowParams);
			if(blueArrowDistance < minBlueArrowDistance)
			{
				minBlueArrowDistance = blueArrowDistance;
				bestBlueArrowObject = blueObject;
			}
			std::cerr << "Odległość niebieskiej strzałki: " << blueArrowDistance << std::endl;
			
			double letterWDistance = calculateEuclidianDistance(moments, parameters.letterWParams);
			if(letterWDistance < minLetterWDistance)
			{
				minLetterWDistance = letterWDistance;
				bestLetterWObject = blueObject;
			}
			std::cerr << "Odległość litery W: " << letterWDistance << std::endl;
			
			double letterKDistance = calculateEuclidianDistance(moments, parameters.letterKParams);
			if(letterKDistance < minLetterKDistance)
			{
				minLetterKDistance = letterKDistance;
				bestLetterKObject = blueObject;
			}
			std::cerr << "Odległość litery K: " << letterKDistance << std::endl;
			
			double letterDDistance = calculateEuclidianDistance(moments, parameters.letterDParams);
			if(letterDDistance < minLetterDDistance)
			{
				minLetterDDistance = letterDDistance;
				bestLetterDObject = blueObject;
			}
			std::cerr << "Odległość litery D: " << letterDDistance << std::endl;
			
		}
	}
	cv::imwrite(tmpDir + "best_blue_arrow.png", bestBlueArrowObject.opencvImage());
	cv::imwrite(tmpDir + "best_letter_w.png", bestLetterWObject.opencvImage());
	cv::imwrite(tmpDir + "best_letter_k.png", bestLetterKObject.opencvImage());
	cv::imwrite(tmpDir + "best_letter_d.png", bestLetterDObject.opencvImage());
	return(0);
}
