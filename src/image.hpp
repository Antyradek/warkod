#pragma once
#include <vector>
#include <stdexcept>

namespace warkod 
{

/// Zachowanie przy odczycie piksela poza granicami obrazu
enum OutOfBoundsBehaviour
{
	/// Zabroń takiego odczytu
	Forbid,
	/// Zwróć wartość zerową
	Zero,
	/// Zwróć wartość najbliższej krawędzi
	Extend
};
	
/// Reprezentuje jeden obraz o wartościach z szablonu
template <typename T>
class Image
{
private:
	/// Szerokość obrazu
	int imageWidth;
	/// Wysokość obrazu
	int imageHeight;
	/// Kontener danych, ustawiony wierszami, więc dostęp jest przez [y][x]
	std::vector<std::vector<T>> imageData;
public:
	/// Zerowy obraz o określonych wymiarach
	Image(int width, int height);
	/// Szerokość obrazu
	int width() const;
	/// Wysokość obrazu
	int height() const;
	/// Wartość piksela w danym punkcie
	T at(int x, int y, OutOfBoundsBehaviour outOfBoundsBehaviour = OutOfBoundsBehaviour::Forbid);
};

template<typename T> 
Image<T>::Image(int width, int height) : 
imageWidth(width), 
imageHeight(height), 
imageData(height, std::vector<T>(width, {1,2,3}))
{
	
}

template<typename T> 
int Image<T>::width() const
{
	return(imageWidth);
}

template<typename T> 
int Image<T>::height() const
{
	return(imageHeight);
}

template<typename T> 
T Image<T>::at(int x, int y, warkod::OutOfBoundsBehaviour outOfBoundsBehaviour)
{
	if(0 < x && x < imageWidth && 0 < y && y < imageHeight)
	{
		return imageData.at(y).at(x);
	}
	else
	{
		switch(outOfBoundsBehaviour)
		{
			case Forbid:
				//wyjątek
				throw std::out_of_range("Piksel poza granicami obrazu");
				break;
			default:
				//wyjątek
				throw std::runtime_error("Niezapimplementowany");
		}
	}
}


}
