#pragma once
#include "filter.hpp"
#include "pixel.hpp"

namespace warkod
{
/// Filtr logiczny dla dylacji obrazu, bierze pod uwagę jedynie 8 sąsiadów
class DilationFilter : public AbstractFilter<BinaryPixel>
{
	
public:
	BinaryPixel operator()(const BinaryPixel& pixel) const override;
};
}
  
