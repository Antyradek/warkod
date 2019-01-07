#pragma once
#include "filter.hpp"
#include "pixel.hpp"

namespace warkod
{
/// Filtr logiczny dla erozji obrazu
class ErosionFilter : public AbstractFilter<BinaryPixel>
{
	
public:
	BinaryPixel operator()(const BinaryPixel& pixel) const override;
};
}
  
 
