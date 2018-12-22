#include "pixel.hpp" 

warkod::ColorfulPixel::ColorfulPixel(double red, double green, double blue) : redValue(red), greenValue(green), blueValue(blue)
{
}

warkod::ColorfulPixel::ColorfulPixel() : ColorfulPixel(0, 0, 0)
{
}

double warkod::ColorfulPixel::red() const
{
	return(redValue);
}

double warkod::ColorfulPixel::blue() const
{
	return(blueValue);
}

double warkod::ColorfulPixel::green() const
{
	return(greenValue);
}



