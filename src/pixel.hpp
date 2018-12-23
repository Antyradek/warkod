#pragma once
#include "image.hpp"
#include <iostream>

namespace warkod
{
	
/// Piksel w obrazie, bez zawartości
template<typename T>
class AbstractPixel
{
protected:
	/// Trzymany obraz
	const Image<T>& image;
	/// Pozycja X w obrazie
	const int positionX;
	/// Pozycja Y w obrazie
	const int positionY;
public:
	/// Piksel o odpowiedniej pozycji w obrazie
	AbstractPixel(const Image<T>& holdingImage, int x, int y);
	/// Zwróć sąsiada o podaną ilość pikseli w bok
	const T& neighbour(int differenceX, int differenceY) const;
	/// Zwróć pozycję piksela
	int imagePositionX() const;
	/// Zwróć pozycję piksela
	int imagePositionY() const;
};

template<typename T> 
const T& AbstractPixel<T>::neighbour(int differenceX, int differenceY) const
{
	const T& val = image.at(positionX + differenceX, positionY + differenceY);
	return(val);
}

template<typename T>
int AbstractPixel<T>::imagePositionX() const
{
	return positionX;
}

template<typename T>
int AbstractPixel<T>::imagePositionY() const
{
	return positionY;
}
	
/// Jeden, konkretny, kolorowy piksel w obrazie
class ColorfulPixel : public AbstractPixel<ColorfulPixel>
{
private:
	/// Wartość czerwona
	double redValue;
	/// Wartość zielona
	double greenValue;
	/// Wartość niebieska
	double blueValue;
	
public:
	
	/// Czarny piksel w obrazie
	ColorfulPixel(const Image<ColorfulPixel>& holdingImage, int x, int y);
	/// Czerwona składowa
	double red() const;
	/// Zielona składowa
	double green() const;
	/// Niebieska składowa
	double blue() const;
	/// Ustaw czerwoną składową
	void red(double value);
	/// Ustaw zieloną składową
	void green(double value);
	/// Ustaw niebieską składową
	void blue(double value);
	/// Wypisz trójkę wartości składowych piksela
	friend std::ostream& operator<<(std::ostream& os, const ColorfulPixel& pixel);
};

/// Binarny piksel w obrazie
class BinaryPixel : public AbstractPixel<BinaryPixel>
{
private:
	/// Wartość piksela
	bool pixelValue;
public:
	/// Zgaszony piksel w obrazie
	BinaryPixel(const Image<BinaryPixel>& holdingImage, int x, int y);
	/// Wypisz wartość piksela
	friend std::ostream& operator<<(std::ostream& os, const BinaryPixel& pixel);
};

}
