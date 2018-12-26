#pragma once
#include "filter.hpp"
#include "pixel.hpp"

namespace warkod
{
/// Filtr medianowy dla kolorowego piksela, dla każdej składowej oblicza osobno
class MedianFilter : public AbstractFilter<ColorfulPixel>
{
private:
	/// Promień filtra kwadratowego, ile pikseli w bok jest brane pod uwagę
	int radius;
	
public:
	ColorfulPixel operator()(const ColorfulPixel& pixel) const override;
	
	/// Filtr kwadratowy o boku @arg squareRadius * 2 + 1
	MedianFilter(int squareRadius);
};
}
 
