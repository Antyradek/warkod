#include "image.hpp" 
#include "pixel.hpp"
#include <cassert>
#include <iostream>
#include <limits>
#include <cstdint>

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
