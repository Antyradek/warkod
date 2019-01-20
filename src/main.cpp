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

/// Punkt na płaszczyźnie
typedef std::pair<double, double> point_t;

/// Para dystansu punktu od najlepszego dopasowania
typedef std::pair<double, point_t> centerDistance_t;

/// Oblicz odległość ważoną obiektu o określonych niezmiennikach od podanego wzoru
double calculateWeightedDistance(const warkod::ObjectFeatures& invariantMoments, const warkod::ObjectParameters& parameters)
{
	const double invariants[10] = {invariantMoments.M1, invariantMoments.M2, invariantMoments.M3, invariantMoments.M4, invariantMoments.M5, invariantMoments.M6, invariantMoments.M7, invariantMoments.M8, invariantMoments.M9, invariantMoments.M10};
	
	double sum = 0;
	for(int i = 0; i < 10; i++)
	{
		sum += std::pow((invariants[i] - parameters.invariants[i]) * parameters.weights[i], 2);
	}
	return(std::sqrt(sum));
}

/// Odległość między dwoma punktami na płaszczyźnie
double calculateDistance(const point_t& pointA, const point_t& pointB)
{
	return(std::sqrt(std::pow(pointA.first - pointB.first, 2) + std::pow(pointA.second - pointB.second, 2)));
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
	warkod::Image<warkod::ColorfulPixel> outputImage(baseImage.width(), baseImage.height());
	
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
		double radius = ((parameters.lightRedRadius - parameters.darkRedRadius)) * pixel.value().red + parameters.darkRedRadius;
		if(std::pow(pixel.value().blue, 2) + std::pow(pixel.value().green, 2) > std::pow(radius, 2))
		{
			//nie mieści się w stożku
			redImage.at(pixel.positionX(), pixel.positionY()).value(false);
		}
		else if(pixel.value().red < parameters.darkRedTreshold)
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
		double radius = ((parameters.lightBlueRadius - parameters.darkBlueRadius)) * pixel.value().blue + parameters.darkBlueRadius;
		if(std::pow(pixel.value().red, 2) + std::pow(pixel.value().green, 2) > std::pow(radius, 2))
		{
			//nie mieści się w stożku
			blueImage.at(pixel.positionX(), pixel.positionY()).value(false);
		}
		else if(pixel.value().blue < parameters.darkBlueTreshold)
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

	outputImage.addImage(blueImage, warkod::Color({0, 0, 0.15}));
	outputImage.addImage(redImage, warkod::Color({0.15, 0, 0}));
	//wyciąganie obiektów z czerwonego obrazu
	std::cerr << "Wyciąganie obrazów z czerwonego..." << std::endl;
	bool found = true;
	int objectCounter = 1;
	//odległości i środki obiektów czerwonej strzałki
	std::vector<centerDistance_t> redArrowDistances;
	warkod::Image<warkod::BinaryPixel> bestRedArrowObject(baseImage.width(), baseImage.height());
	while(found)
	{
		warkod::Image<warkod::BinaryPixel> redObject = redImage.findObject(found);
		if(found)
		{
			std::stringstream ss;
			ss << tmpDir << "obj_" << std::setw(4) << std::setfill('0') << objectCounter << ".png";
			std::cerr << "Wyciągnięto obiekt " << objectCounter << std::endl;
			warkod::ObjectFeatures features = redObject.calculateInvariantMoments();
			//wypisanie parametrów
			std::cout << objectCounter << " ";
			for(int i = 0; i < 10; i++)
			{
				std::cout << features.momentsArray[i] << " ";
			}
			std::cout << features.objectFill;
			std::cout << std::endl;
			
			cv::imwrite(ss.str(), redObject.opencvImage());
			objectCounter++;
			
			if(features.objectFill > parameters.arrowParams.objectFill)
			{
				double redArrowDistance = calculateWeightedDistance (features, parameters.arrowParams);
				redArrowDistances.push_back(centerDistance_t(redArrowDistance, features.imageCenter));
			}
		}
	}
	
	//wyciąganie obiektów z niebieskiego obrazu
	std::cerr << "Wyciąganie obrazów z niebieskiego..." << std::endl;
	found = true;
	std::vector<centerDistance_t> blueArrowDistances;
	std::vector<centerDistance_t> letterWDistances;
	std::vector<centerDistance_t> letterKDistances;
	std::vector<centerDistance_t> letterDDistances;
	while(found)
	{
		warkod::Image<warkod::BinaryPixel> blueObject = blueImage.findObject(found);
		if(found)
		{
			std::stringstream ss;
			ss << tmpDir << "obj_" << std::setw(4) << std::setfill('0') << objectCounter << ".png";
			std::cerr << "Wyciągnięto obiekt " << objectCounter << std::endl;
			warkod::ObjectFeatures features = blueObject.calculateInvariantMoments();
			//wypisz cechy
			std::cout << objectCounter << " ";
			for(int i = 0; i < 10; i++)
			{
				std::cout << features.momentsArray[i] << " ";
			}
			std::cout << features.objectFill;
			std::cout << std::endl;
			cv::imwrite(ss.str(), blueObject.opencvImage());
			objectCounter++;
			
			if(features.objectFill > parameters.arrowParams.objectFill)
			{
				double blueArrowDistance = calculateWeightedDistance(features, parameters.arrowParams);
				blueArrowDistances.push_back(centerDistance_t(blueArrowDistance, features.imageCenter));
			}
			if(features.objectFill > parameters.letterWParams.objectFill)
			{
				double letterWDistance = calculateWeightedDistance(features, parameters.letterWParams);
				letterWDistances.push_back(centerDistance_t(letterWDistance, features.imageCenter));
			}
			if(features.objectFill > parameters.letterKParams.objectFill)
			{
				double letterKDistance = calculateWeightedDistance(features, parameters.letterKParams);
				letterKDistances.push_back(centerDistance_t(letterKDistance, features.imageCenter));
			}
			if(features.objectFill > parameters.letterDParams.objectFill)
			{
				double letterDDistance = calculateWeightedDistance(features, parameters.letterDParams);
				letterDDistances.push_back(centerDistance_t(letterDDistance, features.imageCenter));
			}
			
		}
	}
	//posortuj dopasowania
	struct 
	{
		//specjalna funkcja porównująca porównuje tylko dystans, a nie pozycje środków
		bool operator()(centerDistance_t a, centerDistance_t b) const
		{   
			return(a.first < b.first);
		}   
	} centerDistanceComparator;
	std::sort(redArrowDistances.begin(), redArrowDistances.end(), centerDistanceComparator);
	std::sort(blueArrowDistances.begin(), blueArrowDistances.end(), centerDistanceComparator);
	std::sort(letterWDistances.begin(), letterWDistances.end(), centerDistanceComparator);
	std::sort(letterKDistances.begin(), letterKDistances.end(), centerDistanceComparator);
	std::sort(letterDDistances.begin(), letterDDistances.end(), centerDistanceComparator);
	
	//zaznacz pierwsze dopasowania
	outputImage.markCross(redArrowDistances[0].second, warkod::Color({0.4, 0, 0}), warkod::MarkCrossType::TrippleDotted, 20);
	outputImage.markCross(blueArrowDistances[0].second, warkod::Color({0, 0, 0.4}), warkod::MarkCrossType::TrippleDotted, 20);
	outputImage.markCross(letterWDistances[0].second, warkod::Color({0, 0.3, 0}), warkod::MarkCrossType::TrippleDotted, 20);
	outputImage.markCross(letterKDistances[0].second, warkod::Color({0, 0.6, 0}), warkod::MarkCrossType::TrippleDotted, 20);
	outputImage.markCross(letterDDistances[0].second, warkod::Color({0, 0.9, 0}), warkod::MarkCrossType::TrippleDotted, 20);
	
	//oblicz najbardziej prawdopodobną parę strzałki górnej i dolnej
	//dla każdych kilku najlepszych dopasowań czerwonego, oblicz odległości do najlepszych dopasowań niebieskiego
	//znajdź najmniejszą odległość
	double bestArrowsDistance = std::numeric_limits<double>::max();
	int bestRedArrowIndex = 0;
	int bestBlueArrowIndex = 0;
	for(int redIndex = 0; redIndex < parameters.bestObjectsComparisonDepth; redIndex++)
	{
		for(int blueIndex = 0; blueIndex < parameters.bestObjectsComparisonDepth; blueIndex++)
		{
			double distance = calculateDistance(redArrowDistances[redIndex].second, blueArrowDistances[blueIndex].second);
			if(distance < bestArrowsDistance)
			{
				bestArrowsDistance = distance;
				bestRedArrowIndex = redIndex;
				bestBlueArrowIndex = blueIndex;
			}
		}
	}
	//zaznacz najlepsze dopasowania
	outputImage.markCross(redArrowDistances[bestRedArrowIndex].second, warkod::Color({0.4, 0, 0}), warkod::MarkCrossType::Dotted, 40);
	outputImage.markCross(blueArrowDistances[bestBlueArrowIndex].second, warkod::Color({0, 0, 0.4}), warkod::MarkCrossType::Dotted, 40);
	
	//oblicz środek loga
	point_t logoCenter((redArrowDistances[bestRedArrowIndex].second.first + blueArrowDistances[bestBlueArrowIndex].second.first) / 2, (redArrowDistances[bestRedArrowIndex].second.second + blueArrowDistances[bestBlueArrowIndex].second.second) / 2);
	//zaznacz
	outputImage.markCross(logoCenter, warkod::Color({1, 1, 1}));
	
	cv::imwrite(tmpDir + "output.png", outputImage.opencvImage());
	return(0);
}
