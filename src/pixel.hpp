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
	const int imagePositionX;
	/// Pozycja Y w obrazie
	const int imagePositionY;
public:
	/// Piksel o odpowiedniej pozycji w obrazie
	AbstractPixel(const Image<T>& holdingImage, int x, int y);
	/// Zwróć sąsiada o podaną ilość pikseli w bok
	const T& neighbour(int differenceX, int differenceY, OutOfBoundsBehaviour outOfBoundsBehaviour = OutOfBoundsBehaviour::Forbid) const;
	/// Czy piksel ma sąsiada o określoną ilość pikseli w bok
	bool hasNeighbour(int differenceX, int differenceY) const;
	/// Zwróć pozycję piksela
	int positionX() const;
	/// Zwróć pozycję piksela
	int positionY() const;
	/// Ustaw wartość piksela jako wartość drugiego piksela, coś jak konstruktor kopiujący, ale bez modyfikacji pozycji
	virtual void copyValue(const T& other) = 0;
};

template<typename T> 
const T& AbstractPixel<T>::neighbour(int differenceX, int differenceY, OutOfBoundsBehaviour outOfBoundsBehaviour) const
{
	const T& val = image.at(imagePositionX + differenceX, imagePositionY + differenceY, outOfBoundsBehaviour);
	return(val);
}

template<typename T>
bool AbstractPixel<T>::hasNeighbour(int differenceX, int differenceY) const
{
	return(image.pixelInsideImage(imagePositionX + differenceX, imagePositionY + differenceY));
}

template<typename T>
int AbstractPixel<T>::positionX() const
{
	return imagePositionX;
}

template<typename T>
int AbstractPixel<T>::positionY() const
{
	return imagePositionY;
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
	void copyValue(const ColorfulPixel& other) override;
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
	/// Obraz z każdym zgaszonym pikselem
	BinaryPixel(const Image<BinaryPixel>& holdingImage, int x, int y);
	/// Stan piksela
	bool value() const;
	/// Ustaw stan piksela
	void value(bool value);
	void copyValue(const BinaryPixel& other) override;
	/// Wypisz wartość piksela
	friend std::ostream& operator<<(std::ostream& os, const BinaryPixel& pixel);
};

}
