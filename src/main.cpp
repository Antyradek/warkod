#include <iostream>

int main(int argc, char** argv)
{
	if(argc != 2)
	{
		std::string programName(argv[0]);
		std::cerr << "Użycie: " << programName << " OBRAZ" << std::endl;
		return(-1);
	}
	
	std::string imageFilename = argv[1];
	
	std::cerr << "Przetwarzanie " << imageFilename << std::endl;
	
	return(0);
}
