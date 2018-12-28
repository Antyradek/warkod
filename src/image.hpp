#pragma once
#include "filter.hpp"
#include <opencv2/core/core.hpp>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <iostream>

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
	
/// Niezmienniki momentowe
enum ConstantMoment
{
	Moment1,
	Moment2,
	Moment3,
	Moment4, 
	Moment5,
	Moment6,
	Moment7,
	Moment8,
	Moment9,
	Moment10
};

/// Reprezentuje jeden obraz o wartościach z szablonu
template <typename T>
class Image
{
private:
	/// Szerokość obrazu
	const int imageWidth;
	/// Wysokość obrazu
	const int imageHeight;
	/// Kontener danych, ustawiony wierszami, więc dostęp jest przez [y][x]
	std::vector<std::vector<T>> imageData;
	/// Pozwala iterować piksele kolejno wiersz po wierszu
	class iterator : public std::iterator<std::output_iterator_tag, T>
	{
	private:
		/// Numer piksela, jeśli postawić wiersz za wierszem w buforze
		size_t index;
		/// Iterowany obraz
		Image& thisImage;
	public:
		/// Nowy iterator, idący od początku lub końca
		iterator(Image& image, size_t newIndex = 0);
		/// Następny piksel w wierszu, lub przeskok do następnego wiersza
		iterator& operator++();
		/// Czy iteratory dotyczą tego samego piksela
		bool operator!=(const iterator& other) const;
		/// Zwróć wartość piksela dla tego iteratora
		T& operator*();
	};
	
public:
	/// Zerowy obraz o określonych wymiarach
	Image(int width, int height);
	/// Obraz powstały z obiektu obrazu biblioteki OpenCV
	Image(const cv::Mat& opencvImage);
	/// Szerokość obrazu
	int width() const;
	/// Wysokość obrazu
	int height() const;
	/// Czy piksel z tą pozycją jest w obrazie
	bool pixelInsideImage(int x, int y) const;
	/// Niezmienialna wartość piksela w danym punkcie
	const T& at(int x, int y, OutOfBoundsBehaviour outOfBoundsBehaviour = OutOfBoundsBehaviour::Forbid) const;
	/// Wartość piksela w danym punkcie
	T& at(int x, int y);
	/// Obiekt obrazu biblioteki OpenCV
	cv::Mat opencvImage();
	/// Użyj filtra na całym obrazie
	void applyFilter(const AbstractFilter<T>& filter);
	
	
	/// Wyodrębnij podobiekt z obrazu, usuń znaleziony obiekt z oryginału, ustaw argument
	Image<T> findObject(bool& found);
	/// Oblicz dwuwymiarowy moment (m_{pq})
	double calculateMoment(int p, int q) const;
	/// Oblicz środek ciężkości obrazu
	std::pair<double, double> calculateImageCentre() const;
	/// Oblicz moment centralny obrazu (M_{pq})
	double calculateCentralMoment(int p, int q) const;
	/// Oblicz niezmiennik rzędu <1..10>
	double calculateConstantMoment(ConstantMoment moment) const;
	
	/// Iterator na pierwszy piksel
	iterator begin();
	/// Iterator na ostatni piksel
	iterator end();
};

template<typename T> 
Image<T>::Image(int width, int height) : 
imageWidth(width), 
imageHeight(height)
{
	for(int y = 0; y < imageHeight; y++)
	{
		imageData.push_back(std::vector<T>());
		for(int x = 0; x < imageWidth; x++)
		{
			imageData.at(y).push_back(T(*this, x, y));
		}
	}
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
const T& Image<T>::at(int x, int y, warkod::OutOfBoundsBehaviour outOfBoundsBehaviour) const
{
	if(pixelInsideImage(x, y))
	{
		// at() jest stałą funkcją tylko jeśli ma zwrócić stałą wartość
		return(imageData.at(y).at(x));
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
			//TODO dla Extend stworzyć nowy piksel, nadać odpowiednie wartości i zwrócić
			default:
				//wyjątek
				ss << "Niezapimplementowany " << outOfBoundsBehaviour;
				throw std::runtime_error(ss.str());
		}
	}
}

template<typename T> 
T& Image<T>::at(int x, int y)
{
	if(pixelInsideImage(x, y))
	{
		// at() jest stałą funkcją tylko jeśli ma zwrócić stałą wartość
		return(imageData.at(y).at(x));
	}
	else
	{
		//ponieważ zwracamy modyfikowalną referencję, to nie możemy zwrócić referencji do nieistniejącego obiektu
		std::stringstream ss;
		ss << "Piksel (" << x << "," << y << ") poza granicami obrazu o rozmiarach " << imageWidth << "×" << imageHeight;
		throw std::out_of_range(ss.str());
	}
}


template<typename T>
bool Image<T>::pixelInsideImage(int x, int y) const
{
	return((0 <= x && x < imageWidth && 0 <= y && y < imageHeight));
}

template<typename T>
void Image<T>::applyFilter(const AbstractFilter<T>& filter)
{
	//skopiuj oryginał
	//TODO zmienna original jako const, doimplementować iterator const, żeby było koszernie
	Image<T> original(*this);
	for(const T& pixel : original)
	{
		T filtered = filter(pixel);
		at(pixel.positionX(), pixel.positionY()).copyValue(filtered);
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
T& Image<T>::iterator::operator*()
{
	const int y = index / thisImage.imageWidth;
	const int x = index % thisImage.imageWidth;
	try
	{
		T& val = thisImage.at(x, y);
		return(val);
	}
	catch(const std::out_of_range& err)
	{
		std::cerr << thisImage.imageWidth << " " << thisImage.imageHeight << std::endl;
		std::cerr << x << " " << y << std::endl;
		std::stringstream ss;
		ss << "Indeks iteratora " << index << " jest za duży!";
		throw(std::out_of_range(ss.str()));
	}
}

}
