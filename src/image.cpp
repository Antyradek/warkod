#include "image.hpp" 
#include "pixel.hpp"
#include <cassert>
#include <iostream>

template<> 
warkod::Image<warkod::ColorfulPixel>::Image(const cv::Mat& opencvImage) :
Image(opencvImage.cols, opencvImage.rows)
{
	const cv::Mat_<cv::Vec3b> imageData = opencvImage;
	for(warkod::ColorfulPixel& pixel : *this)
	{
		//piksele w opencv ustawione są BGR
		const int x = pixel.imagePositionX();
		const int y = pixel.imagePositionY();
		assert(x < opencvImage.cols);
		assert(y < opencvImage.rows);
		pixel.red(imageData(y, x)[2]);
		pixel.green(imageData(y, x)[1]);
		pixel.blue(imageData(y, x)[0]);
	}
}

template<> 
cv::Mat warkod::Image<warkod::ColorfulPixel>::opencvImage()
{
	cv::Mat returnedImage(imageHeight, imageWidth, CV_8UC3);
	cv::Mat_<cv::Vec3b> imageData = returnedImage;
	for(const warkod::ColorfulPixel& pixel : *this)
	{
		const int x = pixel.imagePositionX();
		const int y = pixel.imagePositionY();
		//ustawienie pikseli BGR
		imageData(y, x)[0] = pixel.blue();
		imageData(y, x)[1] = pixel.green();
		imageData(y, x)[2] = pixel.red();
	}
	return(returnedImage);
}
