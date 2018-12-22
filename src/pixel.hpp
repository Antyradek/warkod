#pragma once

namespace warkod
{
	
/// Jeden, konkretny, kolorowy piksel w obrazie
class ColorfulPixel
{
private:
	/// Wartość czerwona
	double redValue;
	/// Wartość zielona
	double greenValue;
	/// Wartość niebieska
	double blueValue;
public:
	/// Piksel z określonymi wartościami
	ColorfulPixel(double red, double green, double blue);
	/// Piksel czarny
	ColorfulPixel();
	/// Czerwona składowa
	double red() const;
	/// Zielona składowa
	double green() const;
	/// Niebieska składowa
	double blue() const;
};

}
