#include "pixel.hpp" 

template<typename T>
warkod::AbstractPixel<T>::AbstractPixel(const Image<T>& holdingImage, int x, int y) :
image(holdingImage),
positionX(x),
positionY(y)
{	
}

warkod::ColorfulPixel::ColorfulPixel(const Image<warkod::ColorfulPixel>& holdingImage, int x, int y) :
AbstractPixel(holdingImage, x, y),
redValue(0),
greenValue(0),
blueValue(0)
{
}

double warkod::ColorfulPixel::red() const
{
	return(redValue);
}

void warkod::ColorfulPixel::red(double value)
{
	redValue = value;
}

double warkod::ColorfulPixel::blue() const
{
	return(blueValue);
}

void warkod::ColorfulPixel::blue(double value)
{
	blueValue = value;
}

double warkod::ColorfulPixel::green() const
{
	return(greenValue);
}

void warkod::ColorfulPixel::green(double value)
{
	greenValue = value;
}

namespace warkod
{
	
std::ostream& operator<<(std::ostream& os, const warkod::ColorfulPixel& pixel)
{
	os << "[" << pixel.positionX << "×" << pixel.positionY << "]";
	os << "(" << pixel.red() << "," << pixel.green() << "," << pixel.blue() << ")";
	return os;
}

std::ostream& operator<<(std::ostream& os, const warkod::BinaryPixel& pixel)
{
	os << "[" << pixel.positionX << "×" << pixel.positionY << "]";
	os << "(" << pixel.pixelValue << ")";
	return os;
}
}

warkod::BinaryPixel::BinaryPixel(const Image<warkod::BinaryPixel>& holdingImage, int x, int y) :
AbstractPixel(holdingImage, x, y),
pixelValue(false)
{
}
