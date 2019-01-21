#include <iostream>
#include <fstream>
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

/// Kolorowy obraz
typedef warkod::Image<warkod::ColorfulPixel> ColorfulImage;

/// Binarny obraz
typedef warkod::Image<warkod::BinaryPixel> BinaryImage;

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

/// Sprawdź, czy te wielkości są równe w dopuszczalnym zakresie
bool checkSoftEqual(double first, double second, double softness)
{
	double longer = 0;
	double shorter = 0;
	if(first > second)
	{
		longer = first;
		shorter = second;
	}
	else
	{
		longer = second;
		shorter = first;
	}
	double maxDiff = longer * softness;
	return(shorter + maxDiff > longer);
}

int main(int argc, char** argv)
{
	if(argc != 3 && argc != 4)
	{
		std::string programName(argv[0]);
		std::cerr << "Użycie: " << programName << " OBRAZ WSKAZANY [KATALOG_WYGENEROWANYCH]" << std::endl;
		return(-1);
	}
	
	std::string imageFilename = argv[1];
	std::string outputFilename = argv[2];
	std::string tmpDir = "/tmp/";
	bool generatesDebug = false;
	
	if(argc == 4)
	{
		tmpDir = argv[3];
		tmpDir += "/";
		generatesDebug = true;
	}
	
	warkod::Parameters parameters;
	
	//wczytanie obrazu
	std::cerr << "Wczytywanie " << imageFilename << "... " << std::endl;
	ColorfulImage baseImage(cv::imread(imageFilename));
	if(generatesDebug)
	{
		cv::imwrite(tmpDir + "raw.jpg", baseImage.opencvImage());
	}
	ColorfulImage outputImage(baseImage.width(), baseImage.height());

	
	//zastosowanie filtra medianowego
	std::cerr << "Filtr medianowy..." << std::endl;
	const warkod::MedianFilter medianFilter(parameters.medianFilterRadius);
	baseImage.applyFilter(medianFilter);
	if(generatesDebug)
	{
		cv::imwrite(tmpDir + "median.jpg", baseImage.opencvImage());
	}

	//oddzielenie czerwonego
	std::cerr << "Oddzielanie czerwonego... " << std::endl;
	BinaryImage redImage(baseImage.width(),baseImage.height());
	for(const warkod::ColorfulPixel& pixel : baseImage)
	{
		double radius = ((parameters.lightRedRadius - parameters.darkRedRadius)) * pixel.value().red + parameters.darkRedRadius;
		if(std::pow(pixel.value().blue, 2) + std::pow(pixel.value().green, 2) > std::pow(radius, 2))
		{
			//nie mieści się w stożku
			redImage.at(pixel.positionX(), pixel.positionY()).value(false);
		}
		else if(pixel.value().red < parameters.darkRedThreshold)
		{
			//ciemna część ucięta
			redImage.at(pixel.positionX(), pixel.positionY()).value(false);
		}
		else
		{
			redImage.at(pixel.positionX(), pixel.positionY()).value(true);
		}
	}
	if(generatesDebug)
	{
		cv::imwrite(tmpDir + "red.png", redImage.opencvImage());
	}
	
	//oddzielenie niebieskiego
	std::cerr << "Oddzielanie niebieskiego... " << std::endl;
	BinaryImage blueImage(baseImage.width(),baseImage.height());
	for(const warkod::ColorfulPixel& pixel : baseImage)
	{
		double radius = ((parameters.lightBlueRadius - parameters.darkBlueRadius)) * pixel.value().blue + parameters.darkBlueRadius;
		if(std::pow(pixel.value().red, 2) + std::pow(pixel.value().green, 2) > std::pow(radius, 2))
		{
			//nie mieści się w stożku
			blueImage.at(pixel.positionX(), pixel.positionY()).value(false);
		}
		else if(pixel.value().blue < parameters.darkBlueThreshold)
		{
			//ciemna część ucięta
			blueImage.at(pixel.positionX(), pixel.positionY()).value(false);
		}
		else
		{
			blueImage.at(pixel.positionX(), pixel.positionY()).value(true);
		}
	}
	if(generatesDebug)
	{
		cv::imwrite(tmpDir + "blue.png", blueImage.opencvImage());
	}
	
	//otwieranie
	int openingDepth = parameters.openingDepth * std::min(baseImage.width(), baseImage.height());
	std::cerr << "Otwieranie z głębią " << openingDepth << "..." << std::endl;

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
	if(generatesDebug)
	{
		cv::imwrite(tmpDir + "blue_opened.png", blueImage.opencvImage());
		cv::imwrite(tmpDir + "red_opened.png", redImage.opencvImage());
		outputImage.addImage(blueImage, warkod::Color({0, 0, 0.15}));
		outputImage.addImage(redImage, warkod::Color({0.15, 0, 0}));
	}
	//otwórz plik do zapisywania parametrów
	std::ofstream featuresFile;
	if(generatesDebug)
	{
		featuresFile = std::ofstream(tmpDir + "features.txt", std::ofstream::out);
	}

	//wyciąganie obiektów z czerwonego obrazu
	std::cerr << "Wyciąganie obrazów z czerwonego..." << std::endl;
	bool found = true;
	int objectCounter = 1;
	//odległości i środki obiektów czerwonej strzałki
	std::vector<centerDistance_t> redArrowDistances;
	while(found)
	{
		BinaryImage redObject = redImage.findObject(found);
		if(found)
		{

			std::cerr << "Wyciągnięto obiekt " << objectCounter << std::endl;
			warkod::ObjectFeatures features = redObject.calculateInvariantMoments();
			//wypisanie parametrów
			if(generatesDebug)
			{
				featuresFile << objectCounter << " ";
				for(int i = 0; i < 10; i++)
				{
					featuresFile << features.momentsArray[i] << " ";
				}
				featuresFile << features.objectFill;
				featuresFile << std::endl;
				
				std::stringstream ss;
				ss << tmpDir << "obj_" << std::setw(4) << std::setfill('0') << objectCounter << ".png";
				cv::imwrite(ss.str(), redObject.opencvImage());
			}
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
		BinaryImage blueObject = blueImage.findObject(found);
		if(found)
		{
			std::cerr << "Wyciągnięto obiekt " << objectCounter << std::endl;
			warkod::ObjectFeatures features = blueObject.calculateInvariantMoments();
			if(generatesDebug)
			{
				//wypisz cechy
				featuresFile << objectCounter << " ";
				for(int i = 0; i < 10; i++)
				{
					featuresFile << features.momentsArray[i] << " ";
				}
				featuresFile << features.objectFill;
				featuresFile << std::endl;
				std::stringstream ss;
				ss << tmpDir << "obj_" << std::setw(4) << std::setfill('0') << objectCounter << ".png";
				cv::imwrite(ss.str(), blueObject.opencvImage());
			}
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
	if(generatesDebug)
	{
		featuresFile.close();
	}
	
	//posortuj dopasowania
	std::cerr << "Sortowanie dopasowań..." << std::endl;
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
	if(generatesDebug)
	{
		for(int i = 0; i < parameters.bestObjectsComparisonDepth; i++)
		{
			double radius = 30 - i * (25.0 / parameters.bestObjectsComparisonDepth);
			outputImage.markCross(redArrowDistances[i].second, warkod::Color({0.4, 0, 0}), warkod::MarkCrossType::TrippleDotted, radius);
			outputImage.markCross(blueArrowDistances[i].second, warkod::Color({0, 0, 0.4}), warkod::MarkCrossType::TrippleDotted, radius);
			outputImage.markCross(letterWDistances[i].second, warkod::Color({0.2, 0.2, 0}), warkod::MarkCrossType::TrippleDotted, radius);
			outputImage.markCross(letterKDistances[i].second, warkod::Color({0, 0.6, 0.6}), warkod::MarkCrossType::TrippleDotted, radius);
			outputImage.markCross(letterDDistances[i].second, warkod::Color({0, 1, 0}), warkod::MarkCrossType::TrippleDotted, radius);
		}
	}
	
	//znajdź taką konfigurację obiektów, która najbardziej odpowiada obiektom zawartym w logo
	int bestRedArrowIndex = 0;
	int bestBlueArrowIndex = 0;
	int bestLetterWIndex = 0;
	int bestLetterKIndex = 0;
	int bestLetterDIndex = 0;
	// O(depth^5) juhuuu!!!
	std::cerr << "Szukanie odpowiedniej konfiguracji pozycji obiektów..." << std::endl;
	std::vector<centerDistance_t> configurationDistances;
	for(bestRedArrowIndex = 0; bestRedArrowIndex < parameters.bestObjectsComparisonDepth; bestRedArrowIndex++)
	{
		for(bestBlueArrowIndex = 0; bestBlueArrowIndex < parameters.bestObjectsComparisonDepth; bestBlueArrowIndex++)
		{
			for(bestLetterWIndex = 0; bestLetterWIndex < parameters.bestObjectsComparisonDepth; bestLetterWIndex++)
			{
				for(bestLetterKIndex = 0; bestLetterKIndex < parameters.bestObjectsComparisonDepth; bestLetterKIndex++)
				{
					for(bestLetterDIndex = 0; bestLetterDIndex < parameters.bestObjectsComparisonDepth; bestLetterDIndex++)
					{
						//  R
						//W K D
						//  B
						point_t r = redArrowDistances[bestRedArrowIndex].second;
						point_t b = blueArrowDistances[bestBlueArrowIndex].second;
						point_t w = letterWDistances[bestLetterWIndex].second;
						point_t k = letterKDistances[bestLetterKIndex].second;
						point_t d = letterDDistances[bestLetterDIndex].second;
						double rb = calculateDistance(r, b);
						double rk = calculateDistance(r, k);
						double bk = calculateDistance(b, k);
						double wd = calculateDistance(w, d);
						double wk = calculateDistance(w, k);
						double dk = calculateDistance(d, k);
						double wr = calculateDistance(w, r);
						double dr = calculateDistance(r, d);
						double wb = calculateDistance(w, b);
						double db = calculateDistance(b, d);
						//jeśli jakieś odległości są bliskie zeru, to znaczy że trafiły nam się te same obiekty
						const double minimalDistance = 5.0;
						if(rb < minimalDistance || rk < minimalDistance || bk < minimalDistance || wd < minimalDistance || wk < minimalDistance || dk < minimalDistance || wr < minimalDistance || dr < minimalDistance || wb < minimalDistance || db < minimalDistance)
						{
							continue;
						}
						int points = 0;
						//odległość
						const double distanceGain = std::min(baseImage.width(), baseImage.height()) * parameters.distanceWeight;
						points += distanceGain / (rk * (bestRedArrowIndex + bestLetterKIndex + 2) / 2.0);
						points += distanceGain / (bk * (bestBlueArrowIndex + bestLetterKIndex + 2) / 2.0);
						points += distanceGain / (rb * (bestRedArrowIndex + bestBlueArrowIndex + 2) / 2.0);
						//krzyż
						if(checkSoftEqual(rk + bk, rb, parameters.softComparisonParameter)) points++;
						if(checkSoftEqual(wk + dk, wd, parameters.softComparisonParameter)) points++;
						if(checkSoftEqual(rb, wd, parameters.softComparisonParameter)) points++;
						//boki
						if(checkSoftEqual(wr, db, parameters.softComparisonParameter)) points++;
						if(checkSoftEqual(wb, dr, parameters.softComparisonParameter)) points++;
						//środek
						if(checkSoftEqual(rk, bk, parameters.softComparisonParameter)) points++;
						if(checkSoftEqual(wk, dk, parameters.softComparisonParameter)) points++;
						if(checkSoftEqual(wk, rk, parameters.softComparisonParameter)) points++;
						if(checkSoftEqual(rk, dk, parameters.softComparisonParameter)) points++;
						if(checkSoftEqual( dk, bk, parameters.softComparisonParameter)) points++;
						if(checkSoftEqual(wk, bk, parameters.softComparisonParameter)) points++;
						//deltoid
						if(checkSoftEqual(wr + wb, dr + db, parameters.softComparisonParameter)) points++;
						
						//oblicz środek loga
						point_t logoCenter((redArrowDistances[bestRedArrowIndex].second.first + blueArrowDistances[bestBlueArrowIndex].second.first) / 2, (redArrowDistances[bestRedArrowIndex].second.second + blueArrowDistances[bestBlueArrowIndex].second.second) / 2);
						configurationDistances.push_back(centerDistance_t(points, logoCenter));
					}
				}
			}
		}
	}
	
	std::sort(configurationDistances.begin(), configurationDistances.end(), centerDistanceComparator);
	std::reverse(configurationDistances.begin(), configurationDistances.end());
	std::cerr << "Najlepsza konfiguracja z punktacją " << configurationDistances[0].first << std::endl;
	if(generatesDebug)
	{
		for(int i = 0; i < 5; i++)
		{
			outputImage.markCross(configurationDistances[i].second, warkod::Color({1,1,1}), warkod::MarkCrossType::Dotted, 50 - i * 8);
		}
	}
	point_t logoCenter = configurationDistances[0].second;
	
// 	//sprawdź, czy aby może nie znalazł
// 	if(!foundBestConfig)
// 	{
// 		std::cerr << "Nie znaleziono poprawnej konfiguracji pozycji!";
// 		//ustawiamy na zero i niech się dzieje wola Nieba
// 		bestRedArrowIndex = 0;
// 		bestBlueArrowIndex = 0;
// 		bestLetterWIndex = 0;
// 		bestLetterKIndex = 0;
// 		bestLetterDIndex = 0;
// 	}
	
// 	//zaznacz najlepsze dopasowania
// 	if(generatesDebug)
// 	{
// 		outputImage.markCross(redArrowDistances[bestRedArrowIndex].second, warkod::Color({0.4, 0, 0}), warkod::MarkCrossType::Dotted, 40);
// 		outputImage.markCross(blueArrowDistances[bestBlueArrowIndex].second, warkod::Color({0, 0, 0.4}), warkod::MarkCrossType::Dotted, 40);
// 		outputImage.markCross(letterWDistances[bestLetterWIndex].second, warkod::Color({0, 0.3, 0}), warkod::MarkCrossType::Dotted, 40);
// 		outputImage.markCross(letterKDistances[bestLetterKIndex].second, warkod::Color({0, 0.6, 0}), warkod::MarkCrossType::Dotted, 40);
// 		outputImage.markCross(letterDDistances[bestLetterDIndex].second, warkod::Color({0, 0.9, 0}), warkod::MarkCrossType::Dotted, 40);
// 	}
	
	
	//zaznacz
	if(generatesDebug)
	{
		outputImage.markCross(logoCenter, warkod::Color({1, 1, 1}));
		cv::imwrite(tmpDir + "output.png", outputImage.opencvImage());
	}
	ColorfulImage rawImage(cv::imread(imageFilename));
	rawImage.markCross(logoCenter, warkod::Color({1,1,1}));
	cv::imwrite(outputFilename, rawImage.opencvImage());
	std::cout << "Wynik: " << logoCenter.first << " " << logoCenter.second << std::endl;
	return(0);
}
