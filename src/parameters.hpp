#pragma once

namespace warkod
{
	
/// Parametry do znalezienia jednego obiektu na obrazie
struct ObjectParameters
{
	/// Niezmienniki M1..M10
	double invariants[10];
	/// Odchylenie standardowe
	double sigmas[10];
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
	
	ObjectParameters arrowParams = {{ 0, 0, 0, 0.00125014, 0, 0.000190286, 0, 0, -4.33119e-05, 0 }, { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,0,0,1,0,1,0,0,1.5,0 }, 0.002};
	ObjectParameters letterWParams = {{ 0, 0, 0, 0, 0, -2.54905e-05, 0, 0, -9.10516e-06, 0 }, { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,0,0,0,0,0.9,0,0,1,0 }, 0.00045};
	ObjectParameters letterKParams = {{ 0, 0, 0, 0.000282899, 0, 0, 0, -8.39919e-06, -2.10566e-06, 0 }, { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,0,0,1,0,0,0,1,1,0 }, 0.00035};
	ObjectParameters letterDParams = {{ 0, 0, 0, 0, 0, 0, 0, -5.53758e-05, -5.55874e-06, 0 }, { 1, 1, 1, 1, 1, 1, 1, 0.5, 0.5, 1 }, { 0,0,0,0,0,0,0,1,1,0 }, 0.0004};

};
}
