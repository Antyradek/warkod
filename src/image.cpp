#include "image.hpp"
#include "pixel.hpp"
#include <cassert>
#include <iostream>
#include <limits>
#include <cstdint>
#include <queue>
#include <utility>
#include <initializer_list>
#include <algorithm>

/// Wczytaj kolorowy obraz
template<>
warkod::Image<warkod::ColorfulPixel>::Image(const cv::Mat& opencvImage) :
    Image(opencvImage.cols, opencvImage.rows)
{
    const cv::Mat_<cv::Vec3b> imageData = opencvImage;
    for(warkod::ColorfulPixel& pixel : *this)
    {
        //piksele w opencv ustawione są BGR
        const int x = pixel.positionX();
        const int y = pixel.positionY();
        assert(x < opencvImage.cols);
        assert(y < opencvImage.rows);
        double red = static_cast<double>(imageData(y, x)[2]) / std::numeric_limits<uint8_t>::max();
        double green = static_cast<double>(imageData(y, x)[1]) / std::numeric_limits<uint8_t>::max();
        double blue = static_cast<double>(imageData(y, x)[0]) / std::numeric_limits<uint8_t>::max();
		pixel.value(warkod::Color({red, green, blue}));
    }
}

/// Generuj kolorowy obraz
template<>
cv::Mat warkod::Image<warkod::ColorfulPixel>::opencvImage()
{
    cv::Mat returnedImage(imageHeight, imageWidth, CV_8UC3);
    cv::Mat_<cv::Vec3b> imageData = returnedImage;
    for(const warkod::ColorfulPixel& pixel : *this)
    {
        const int x = pixel.positionX();
        const int y = pixel.positionY();
        //ustawienie pikseli BGR
        imageData(y, x)[0] = pixel.value().blue * std::numeric_limits<uint8_t>::max();
        imageData(y, x)[1] = pixel.value().green * std::numeric_limits<uint8_t>::max();
        imageData(y, x)[2] = pixel.value().red * std::numeric_limits<uint8_t>::max();
    }
    return(returnedImage);
}

template<>
cv::Mat warkod::Image<warkod::BinaryPixel>::opencvImage()
{
    cv::Mat returnedImage(imageHeight, imageWidth, CV_8UC3);
    cv::Mat_<cv::Vec3b> imageData = returnedImage;
    for(const warkod::BinaryPixel& pixel : *this)
    {
        uint8_t pixelValue = pixel.value() * std::numeric_limits<uint8_t>::max();
        imageData(pixel.positionY(), pixel.positionX())[0] = pixelValue;
        imageData(pixel.positionY(), pixel.positionX())[1] = pixelValue;
        imageData(pixel.positionY(), pixel.positionX())[2] = pixelValue;
    }
    return(returnedImage);
}

template<>
warkod::Image<warkod::BinaryPixel> warkod::Image<warkod::BinaryPixel>::findObject(bool& found)
{
    //tworzymy pusty obraz o takich samych rozmiarach
    warkod::Image<warkod::BinaryPixel> objectImage(width(), height());
    //zbiór pikseli do rozlania
    std::queue<warkod::BinaryPixel> fillQueue;

    //szukamy pierwszego włączonego piksela i dodajemy do pikseli do rozlania
    for(const warkod::BinaryPixel& pixel : *this)
    {
        if(pixel.value())
        {
            fillQueue.push(pixel);
            found = true;
            break;
        }
    }

    //jeśli nie znalazł pierwszego piksela, to znaczy że nie ma więcej obiektów do wykrycia
    if(fillQueue.empty())
    {
        found = false;
        return(objectImage);
    }

    //główna pętla
    while(!fillQueue.empty())
    {
        //rozlej piksel na sąsiednie, jeśli w oryginale są włączone
        //dodaj je do kolejki
        //usuń z oryginału
        const warkod::BinaryPixel& fillingPixel = fillQueue.front();
        //sąsiedzi są plusem bez środka
        const std::vector<std::pair<int, int>> neighbours({{-1,0},{1,0},{0,-1},{0,1}});
        for(const std::pair<int, int>& pair : neighbours)
        {
            if(fillingPixel.hasNeighbour(pair.first, pair.second) && fillingPixel.neighbour(pair.first, pair.second).value())
            {
                //dodaj do kolejki
                fillQueue.push(fillingPixel.neighbour(pair.first, pair.second));
                //usuń dodany do kolejki z oryginału
                at(fillingPixel.neighbour(pair.first, pair.second).positionX(), fillingPixel.neighbour(pair.first, pair.second).positionY()).value(false);
            }
        }
        //oznacz rozlewany piksel w obrazie obiektu
        assert(objectImage.at(fillingPixel.positionX(), fillingPixel.positionY()).value() == false);
        objectImage.at(fillingPixel.positionX(), fillingPixel.positionY()).value(true);
        //usuń rozlewany piksel z oryginału
        //NOTE fillingPixel jest kopią obiektu, więc zmiana jego wartości nie spowoduje zmiany obrazu
        at(fillingPixel.positionX(), fillingPixel.positionY()).value(false);
        //usuń z bufora rozlewanych pikseli
        fillQueue.pop();
    }
    return(objectImage);
}


template<>
warkod::Moments warkod::Image<warkod::BinaryPixel>::calculateMoments() const
{
    Moments ret = {};
    for(int i = 1; i <= width(); i++)
    {
        for(int j = 1; j <= height(); j++)
        {
            //i i j są liczone od 1, ale piksele są oznaczane od 0
            bool value = at(i - 1, j - 1).value();
            ret.m00 += value;
            ret.m01 += j * value;
            ret.m10 += i * value;
        }
    }
    return(ret);
}

template<>
std::pair<double, double> warkod::Image<warkod::BinaryPixel>::calculateImageCenter() const
{
    std::pair<double, double> response;
    Moments moments = calculateMoments();
    response.first = moments.m10 / moments.m00;
    response.second = moments.m01 / moments.m00;
    return(response);
}


template<>
warkod::NormalisedCentralMoments warkod::Image<warkod::BinaryPixel>::calculateNormalisedCentralMoments() const
{
    warkod::Moments moments = calculateMoments();
    std::pair<double, double> imageCenter;
    imageCenter.first = moments.m10 / moments.m00;
    imageCenter.second = moments.m01 / moments.m00;

    warkod::NormalisedCentralMoments ret = {};

    //NOTE taniej jest liczyć wszystkie potrzebne momenty centralne jednocześnie, niż dla każdego dawać metodę i przelatywać obraz osobno
    for(int i = 1; i <= width(); i++)
    {
        for(int j = 1; j <= height(); j++)
        {
            //i i j są liczone od 1, ale tablica obrazu jest liczona od 0
            const double diffX = i - imageCenter.first;
            const double diffY = j - imageCenter.second;
            const double value = at(i - 1, j - 1).value();
            const double div = moments.m00;
			const double div2 = div * div;
			const double div3 = std::pow(div, 2.5);

            // N_{pq} = M_{pq} / m_{00}^{(p+q)/2+1}
            ret.N02 += diffY * diffY * value / div2;
            ret.N20 += diffX * diffX * value / div2;
			ret.N11 += diffX * diffY * value / div2;
			ret.N03 += diffY * diffY * diffY * value / div3;
			ret.N30 += diffX * diffX * diffX * value / div3;
			ret.N12 += diffX * diffY * diffY * value / div3;
			ret.N21 += diffX * diffX * diffY * value / div3;

        }
    }
    return(ret);
}



template<>
warkod::ObjectFeatures warkod::Image<warkod::BinaryPixel>::calculateInvariantMoments() const
{
	warkod::Moments moments = calculateMoments();
    std::pair<double, double> imageCenter;
    imageCenter.first = moments.m10 / moments.m00;
    imageCenter.second = moments.m01 / moments.m00;

    warkod::NormalisedCentralMoments ncs = {};

    //NOTE taniej jest liczyć wszystkie potrzebne momenty centralne jednocześnie, niż dla każdego dawać metodę i przelatywać obraz osobno
    for(int i = 1; i <= width(); i++)
    {
        for(int j = 1; j <= height(); j++)
        {
            //i i j są liczone od 1, ale tablica obrazu jest liczona od 0
            const double diffX = i - imageCenter.first;
            const double diffY = j - imageCenter.second;
            const double value = at(i - 1, j - 1).value();
            const double div = moments.m00;
			const double div2 = div * div;
			const double div3 = std::pow(div, 2.5);

            // N_{pq} = M_{pq} / m_{00}^{(p+q)/2+1}
            ncs.N02 += diffY * diffY * value / div2;
            ncs.N20 += diffX * diffX * value / div2;
			ncs.N11 += diffX * diffY * value / div2;
			ncs.N03 += diffY * diffY * diffY * value / div3;
			ncs.N30 += diffX * diffX * diffX * value / div3;
			ncs.N12 += diffX * diffY * diffY * value / div3;
			ncs.N21 += diffX * diffX * diffY * value / div3;

        }
    }
	
    warkod::ObjectFeatures ret = {};

    ret.M1 = ncs.N20 + ncs.N02;
	ret.M2 = (ncs.N20 - ncs.N02) * (ncs.N20 - ncs.N02) + 4 * ncs.N11 * ncs.N11;
	ret.M3 = (ncs.N30 - 3 * ncs.N12) * (ncs.N30 - 3 * ncs.N12) + (3 * ncs.N21 - ncs.N03) * (3 * ncs.N21 - ncs.N03);
	ret.M4 = std::pow(ncs.N30 + ncs.N12, 2) + std::pow(ncs.N21 + ncs.N03, 2);
	ret.M5 = (ncs.N30 - 3 * ncs.N12) * (ncs.N30 + ncs.N12) * ((ncs.N30 + ncs.N12) * (ncs.N30 + ncs.N12) - 3 * (ncs.N21 + ncs.N03) * (ncs.N21 + ncs.N03)) + (3 * ncs.N21 - ncs.N03) * (ncs.N21 + ncs.N03) * (3 * (ncs.N30 + ncs.N12) * (ncs.N30 + ncs.N12) - (ncs.N21 + ncs.N03) * (ncs.N21 + ncs.N03));
	ret.M6 = (ncs.N20 - ncs.N02) * ((ncs.N30 + ncs.N12) * (ncs.N30 + ncs.N12) - (ncs.N21 + ncs.N03) * (ncs.N21 + ncs.N03)) + 4 * ncs.N11 * (ncs.N30 + ncs.N12) * (ncs.N21 + ncs.N03);
	ret.M7 = ncs.N20 * ncs.N02 - ncs.N11 * ncs.N11;
	ret.M8 = ncs.N30 * ncs.N12 + ncs.N21 * ncs.N03 - ncs.N12 * ncs.N12 - ncs.N21 * ncs.N21;
	ret.M9 = ncs.N20 * (ncs.N21 * ncs.N03 - ncs.N12 * ncs.N12) + ncs.N02 * (ncs.N30 * ncs.N12 - ncs.N21 * ncs.N21) - ncs.N11 * (ncs.N30 * ncs.N03 - ncs.N21 * ncs.N12);
	ret.M10 = (ncs.N30 * ncs.N03 - ncs.N12 * ncs.N21) * (ncs.N30 * ncs.N03 - ncs.N12 * ncs.N21) - 4 * (ncs.N30 * ncs.N12 - ncs.N21 * ncs.N21) * (ncs.N03 * ncs.N21 - ncs.N12);

	//kopiuje wartości do tablicy
	std::initializer_list<double> initList({ret.M1, ret.M2, ret.M3, ret.M4, ret.M5, ret.M6, ret.M7, ret.M8, ret.M9, ret.M10});
	std::copy(initList.begin(), initList.end(), ret.momentsArray);
	
	ret.imageCenter = imageCenter;
	ret.objectFill = static_cast<double>(moments.m00) / (width() * height());
	
    return(ret);
}

template <>
template <>
void warkod::Image<warkod::ColorfulPixel>::addImage(warkod::Image<warkod::BinaryPixel>& other, const warkod::Color& tint)
{
	for(warkod::BinaryPixel& pixel : other)
	{
		if(pixel.value())
		{
			at(pixel.positionX(), pixel.positionY()).value(tint);
		}
	}
}
