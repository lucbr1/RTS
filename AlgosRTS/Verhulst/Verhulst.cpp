#include <iostream>
#include <fstream>
#include <cmath>

double a = 0.05; // taux de reproduction
double b = 0.01; // taux de mortalité
unsigned int dt = 1; // pas de temps
double populationDepart = 2; // population initiale
std::ofstream fichier; // fichier pour stocker les résultats

double CalculerPopulationImplicite(double Pn, double& a, double& b, unsigned int& dt)
{
	return (-(1-(a*dt)) + std::sqrt((1-(a*dt))*(1-(a*dt)) + 4*b*dt*Pn)) / (2*b*dt);
}

double CalculerPopulation(double Pn, double& a, double& b, unsigned int& dt)
{
	return Pn * (1 + dt * (a - b * Pn));
}

double calculerEvolutionPopulation(double Pn, double& a, double& b, unsigned int& dt, unsigned int n)
{
	for (unsigned int i = 1; i < n; i++)
	{
		/// ------------------------------------------------
		/// Choisir entre implicite et explicite en commentant/décommentant les lignes suivantes
		/// ------------------------------------------------
		Pn = CalculerPopulationImplicite(Pn, a, b, dt);
		// Pn = CalculerPopulation(Pn, a, b, dt);
		std::cout << "Population au jour " << i * dt << " : " << Pn << std::endl;

		fichier << i * dt << ";" << Pn << ";" << "\n";
	}
	return Pn;
}

int main()
{
	fichier.open("populationVerhulst.csv");

	if (!fichier.is_open())
	{
		std::cerr << "Erreur lors de l'ouverture du fichier." << std::endl;
		return 1;
	}

	fichier << "Jour;Population;\n";
	fichier << "0;" << populationDepart << ";\n";

	calculerEvolutionPopulation(populationDepart, a, b, dt, 100);

	fichier.close();
}