#pragma once

namespace warkod
{
	
/// Parametry do znalezienia jednego obiektu na obrazie
struct ObjectParameters
{
	/// Niezmienniki M1..M10
	double invariants[10];
	/// Wagi współczynników
	double weights[10];
	/// Wielkość obiektu, jako ułamek wypełnienia obrazu
	double objectFill;
};
	
/// Parametry algorytmów
struct Parameters
{
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
	int medianFilterRadius = 3;
	/// Szerokość otwarcia jako ułamek węższego boku obrazu, będzie zaokrąglone do całkowitych ilości pikseli
	double openingWidth = 0.003;
	/// Ilość najlepszych dopasowań do porównania między sobą
	int bestObjectsComparisonDepth = 3;
	
	ObjectParameters arrowParams = {{ 0, 0, 0, 0.00115014, 0, 0.000160286, 0, 0, -4.53119e-05, 0 }, { 0,0,0,0.25,0,0.8,0,0,0.25,0 }, 0.002};
	ObjectParameters letterWParams = {{ 0, 0, 0, 0, 0, -3.54905e-05, 0, 0, -2.10516e-05, 0 }, { 0,0,0,0,0,0.25,0,0,2.25,0 }, 0.002};
	ObjectParameters letterKParams = {{ 0, 0, 0, 0.000252899, 0, 0, 0, -3.99919e-05, -3.30566e-06, 0 }, { 0,0,0,0.8,0,0,0,0.8,1.8,0 }, 0.001};
	ObjectParameters letterDParams = {{ 0, 0, 0, 0, 0, 0, 0, -5.13758e-05, -5.55874e-06, 0 }, { 0,0,0,0,0,0,0,1,0.6,0 }, 0.001};

};
}
