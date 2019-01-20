#include "pixel.hpp" 

template<typename T>
warkod::AbstractPixel<T>::AbstractPixel(const Image<T>& holdingImage, int x, int y) :
image(holdingImage),
imagePositionX(x),
imagePositionY(y)
{	
}

warkod::ColorfulPixel::ColorfulPixel(const Image<warkod::ColorfulPixel>& holdingImage, int x, int y) :
AbstractPixel(holdingImage, x, y),
redValue(0),
greenValue(0),
blueValue(0)
{
}

namespace warkod
{
	
std::ostream& operator<<(std::ostream& os, const warkod::ColorfulPixel& pixel)
{
	os << "[" << pixel.positionX() << "×" << pixel.positionY() << "]";
	os << "(" << pixel.value().red << "," << pixel.value().green << "," << pixel.value().blue << ")";
	return os;
}

std::ostream& operator<<(std::ostream& os, const warkod::BinaryPixel& pixel)
{
	os << "[" << pixel.positionX() << "×" << pixel.positionY() << "]";
	os << "(" << pixel.pixelValue << ")";
	return os;
}
}

warkod::BinaryPixel::BinaryPixel(const Image<warkod::BinaryPixel>& holdingImage, int x, int y) :
AbstractPixel(holdingImage, x, y),
pixelValue(false)
{
}

bool warkod::BinaryPixel::value() const
{
	return(pixelValue);
}

void warkod::BinaryPixel::value(bool value)
{
	pixelValue = value;
}

warkod::Color warkod::ColorfulPixel::value() const
{
	return(colorValue);
}

void warkod::ColorfulPixel::value(const warkod::Color& newValue)
{
	colorValue = newValue;
}
