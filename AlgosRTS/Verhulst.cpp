#include <iostream>
#include <fstream>

double a = 0.05; // taux de reproduction
double b = 0.01; // taux de mortalité
unsigned int dt = 1; // pas de temps
double populationDepart = 2; // population initiale
std::ofstream fichier; // fichier pour stocker les résultats

double CalculerPopulation(double Pn, double a, double b, unsigned int dt)
{
    return Pn * (1 + dt * (a - b * Pn));
}

double calculerEvolutionPopulation(double Pn, double a, double b, unsigned int dt, unsigned int n)
{
	for (unsigned int i = 0; i < n; i++)
	{
		Pn = CalculerPopulation(Pn, a, b, dt);
		std::cout << "Population a l'iteration " << i * dt << " : " << Pn << std::endl;

		if (fichier.is_open())
		{
			fichier << i * dt << ";" << Pn << ";" << "\n";
		}
	}
	return Pn;
}

int main()
{
	///-----------------------------------------------
	/// Demander si entier ou double (car monde discret)
	/// ----------------------------------------------
	fichier.open("population.csv");

	calculerEvolutionPopulation(populationDepart, a, b, dt, 100);

	fichier.close();
}

