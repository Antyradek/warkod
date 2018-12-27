#include "image.hpp" 
#include "pixel.hpp"
#include <cassert>
#include <iostream>
#include <limits>
#include <cstdint>
#include <queue>
#include <utility>

/// Wczytaj kolorowy obraz
template<> 
warkod::Image<warkod::ColorfulPixel>::Image(const cv::Mat& opencvImage) :
Image(opencvImage.cols, opencvImage.rows)
{
	const cv::Mat_<cv::Vec3b> imageData = opencvImage;
	for(warkod::ColorfulPixel& pixel : *this)
	{
		//piksele w opencv ustawione są BGR
		const int x = pixel.positionX();
		const int y = pixel.positionY();
		assert(x < opencvImage.cols);
		assert(y < opencvImage.rows);
		pixel.red(static_cast<double>(imageData(y, x)[2]) / std::numeric_limits<uint8_t>::max());
		pixel.green(static_cast<double>(imageData(y, x)[1]) / std::numeric_limits<uint8_t>::max());
		pixel.blue(static_cast<double>(imageData(y, x)[0]) / std::numeric_limits<uint8_t>::max());
	}
}

/// Generuj kolorowy obraz
template<> 
cv::Mat warkod::Image<warkod::ColorfulPixel>::opencvImage()
{
	cv::Mat returnedImage(imageHeight, imageWidth, CV_8UC3);
	cv::Mat_<cv::Vec3b> imageData = returnedImage;
	for(const warkod::ColorfulPixel& pixel : *this)
	{
		const int x = pixel.positionX();
		const int y = pixel.positionY();
		//ustawienie pikseli BGR
		imageData(y, x)[0] = pixel.blue() * std::numeric_limits<uint8_t>::max();
		imageData(y, x)[1] = pixel.green() * std::numeric_limits<uint8_t>::max();
		imageData(y, x)[2] = pixel.red() * std::numeric_limits<uint8_t>::max();
	}
	return(returnedImage);
}

template<> 
cv::Mat warkod::Image<warkod::BinaryPixel>::opencvImage()
{
	cv::Mat returnedImage(imageHeight, imageWidth, CV_8UC3);
	cv::Mat_<cv::Vec3b> imageData = returnedImage;
	for(const warkod::BinaryPixel& pixel : *this)
	{
		uint8_t pixelValue = pixel.value() * std::numeric_limits<uint8_t>::max();
		imageData(pixel.positionY(), pixel.positionX())[0] = pixelValue;
		imageData(pixel.positionY(), pixel.positionX())[1] = pixelValue;
		imageData(pixel.positionY(), pixel.positionX())[2] = pixelValue;
	}
	return(returnedImage);
}

template<> 
warkod::Image<warkod::BinaryPixel> warkod::Image<warkod::BinaryPixel>::findObject(bool& found)
{
	//tworzymy pusty obraz o takich samych rozmiarach
	warkod::Image<warkod::BinaryPixel> objectImage(width(), height());
	//zbiór pikseli do rozlania
	std::queue<warkod::BinaryPixel> fillQueue;
	
	//szukamy pierwszego włączonego piksela i dodajemy do pikseli do rozlania
	for(const warkod::BinaryPixel& pixel : *this)
	{
		if(pixel.value())
		{
			fillQueue.push(pixel);
			found = true;
			break;
		}
	}
	
	//jeśli nie znalazł pierwszego piksela, to znaczy że nie ma więcej obiektów do wykrycia
	if(fillQueue.empty())
	{
		found = false;
		return(objectImage);
	}
	
	//główna pętla
	while(!fillQueue.empty())
	{
		//rozlej piksel na sąsiednie, jeśli w oryginale są włączone
		//dodaj je do kolejki
		//usuń z oryginału
		const warkod::BinaryPixel& fillingPixel = fillQueue.front();
		//sąsiedzi są plusem bez środka
		const std::vector<std::pair<int, int>> neighbours({{-1,0},{1,0},{0,-1},{0,1}});
		for(const std::pair<int, int>& pair : neighbours)
		{
			if(fillingPixel.hasNeighbour(pair.first, pair.second) && fillingPixel.neighbour(pair.first, pair.second).value())
			{
				//dodaj do kolejki
				fillQueue.push(fillingPixel.neighbour(pair.first, pair.second));
				//usuń dodany do kolejki z oryginału
				at(fillingPixel.neighbour(pair.first, pair.second).positionX(), fillingPixel.neighbour(pair.first, pair.second).positionY()).value(false);
			}
		}
		//oznacz rozlewany piksel w obrazie obiektu
		assert(objectImage.at(fillingPixel.positionX(), fillingPixel.positionY()).value() == false);
		objectImage.at(fillingPixel.positionX(), fillingPixel.positionY()).value(true);
		//usuń rozlewany piksel z oryginału
		//NOTE fillingPixel jest kopią obiektu, więc zmiana jego wartości nie spowoduje zmiany obrazu
		at(fillingPixel.positionX(), fillingPixel.positionY()).value(false);
		//usuń z bufora rozlewanych pikseli
		fillQueue.pop();
	}
	return(objectImage);
}
