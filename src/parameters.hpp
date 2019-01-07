#pragma once

namespace warkod
{
/// Parametry algorytmów
class Parameters
{
public:
	/// Średnica czerwonego stożka od strony czarnej
	double darkRedRadius = 0.3;
	/// Średnica czerwonego stożka od strony jasnej
	double lightRedRadius = 0.7;
	/// Próg czerni czerwonego stożka
	double darkRedTreshold = 0.2;
	/// Średnica niebieskiego stożka od strony czarnej
	double darkBlueRadius = 0.3;
	/// Średnica niebieskiego stożka od strony jasnej
	double lightBlueRadius = 0.9;
	/// Próg czerni niebieskiego stożka
	double darkBlueTreshold = 0.1;
	/// Promień działania pola filtra medianowego
	int medianFilterRadius = 2;
	/// Szerokość otwarcia jako ułamek węższego boku obrazu, będzie zaokrąglone do całkowitych ilości pikseli
	double openingWidth = 0.003;
};
}
