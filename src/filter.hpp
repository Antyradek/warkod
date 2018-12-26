#pragma once

namespace warkod
{
/// Jakiś filtr, który dla każdego piksela wykonuje pewną funkcję
template<typename T>
class AbstractFilter
{
public:
	/// Wywołaj dla piksela, kopiuje wartości i zwraca nowy piksel
	virtual T operator()(const T& pixel) const = 0;
};

}
