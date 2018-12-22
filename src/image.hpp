#pragma once
#include <vector>
#include <stdexcept>
#include <sstream>

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
	
	class iterator : public std::iterator<std::output_iterator_tag, T>
	{
	private:
		/// Numer piksela, jeśli postawić wiersz za wierszem w buforze
		size_t index;
		/// Iterowany obraz
		Image& thisImage;
	public:
		/// Nowy iterator, idący od początku
		iterator(Image& image, size_t newIndex = 0);
		/// Następny piksel w wierszu, lub przeskok do następnego wiersza
		iterator& operator++();
		/// Czy iteratory dotyczą tego samego piksela
		bool operator!=(const iterator& other) const;
		/// Zwróć wartość piksela dla tego iteratora
		T operator*() const;
	};
	
public:
	/// Zerowy obraz o określonych wymiarach
	Image(int width, int height);
	/// Szerokość obrazu
	int width() const;
	/// Wysokość obrazu
	int height() const;
	/// Wartość piksela w danym punkcie
	T at(int x, int y, OutOfBoundsBehaviour outOfBoundsBehaviour = OutOfBoundsBehaviour::Forbid);
	
	/// Iterator na pierwszy piksel
	iterator begin();
	/// Iterator na ostatni piksel
	iterator end();
	
	
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
	if(0 <= x && x < imageWidth && 0 <= y && y < imageHeight)
	{
		return imageData.at(y).at(x);
	}
	else
	{
		std::stringstream ss;
		switch(outOfBoundsBehaviour)
		{
			case Forbid:
				//wyjątek
				ss << "Piksel (" << x << "," << y << ") poza granicami obrazu o rozmiarach " << imageWidth << "×" << imageHeight;
				throw std::out_of_range(ss.str());
				break;
			default:
				//wyjątek
				ss << "Niezapimplementowany " << outOfBoundsBehaviour;
				throw std::runtime_error(ss.str());
		}
	}
}

template<typename T> 
Image<T>::iterator::iterator(Image<T>& image, size_t newIndex) :
index(newIndex),
thisImage(image)
{
}

template<typename T> 
typename Image<T>::iterator& Image<T>::iterator::operator++()
{
	index++;
	return(*this);
}

template<typename T> 
typename Image<T>::iterator Image<T>::begin()
{
	return(iterator(*this));
}

template<typename T> 
typename Image<T>::iterator Image<T>::end()
{
	iterator iter(*this, imageWidth * imageHeight);
	return iter;
}

template<typename T> 
bool Image<T>::iterator::operator!=(const Image<T>::iterator& other) const
{
	return(index != other.index);
}

template<typename T> 
T Image<T>::iterator::operator*() const
{
	const int y = index / thisImage.imageHeight;
	const int x = index % thisImage.imageWidth;
	return(thisImage.at(x, y));
}



}
