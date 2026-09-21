#include <iostream>
#include <fstream>
#include <tuple>

double alpha = 0.05; // taux de reproduction des proies
double beta = 0.001; // taux de mortalité des proies dû aux prédateurs
double gamma = 0.05; // taux de mortalité naturelle des prédateurs
double delta = 0.001; // taux de reproduction des prédateurs grâce aux proies consommées

unsigned int dt = 1; // pas de temps

double populationHerbivoreInit = 50; // population initiale
double populationCarnivoreInit = 100; // population initiale

std::ofstream fichier; // fichier pour stocker les résultats

std::tuple<double, double> CalculerPopulations(double Hn, double Cn, double& alpha, double& beta, double& gamma, double& delta, unsigned int& dt)
{
	double H = Hn * (dt * (alpha - beta * Cn) + 1);
	if (H < 0.0) H = 0.0; // Empêche l'oscillation

	double C = Cn * (dt * (-gamma + delta * Hn) + 1);
	if (C < 0.0) C = 0.0; // Empêche l'oscillation)

	return std::make_tuple(H, C);
}

std::tuple<double, double> CalculerPopulationsSemiImplicite(double Hn, double Cn, double& alpha, double& beta, double& gamma, double& delta, unsigned int& dt)
{
	double H = Hn * (1 + dt * (alpha - beta * Cn));
	if (H < 0.0) H = 0.0; // Empêche l'oscillation

	double C = Cn * (1 + dt * (delta * H - gamma));
	if (C < 0.0) C = 0.0; // Empêche l'oscillation

	return std::make_tuple(H, C);
}

std::tuple<double, double> calculerEvolutionPopulations(double Hn, double Cn, double& alpha, double& beta, double& gamma, double& delta, unsigned int& dt, unsigned int n)
{
	for (unsigned int i = 1; i < n; i++)
	{
		/// ------------------------------------------------
		/// Choisir entre implicite et explicite en commentant/décommentant les lignes suivantes
		/// ------------------------------------------------
		std::tie(Hn, Cn) = CalculerPopulationsSemiImplicite(Hn, Cn, alpha, beta, gamma, delta, dt);
		// std::tie(Hn, Cn) = CalculerPopulations(Hn, Cn, alpha, beta, gamma, delta, dt);
		std::cout << "Population au jour " << i * dt << " : " << Hn << ", " << Cn << std::endl;

		fichier << i * dt << ";" << Hn << ";" << Cn << ";\n";
	}
	return std::make_tuple(Hn, Cn);
}

int main()
{
	fichier.open("populationLotkaVolterra.csv");

	if (!fichier.is_open())
	{
		std::cerr << "Erreur lors de l'ouverture du fichier." << std::endl;
		return 1;
	}

	fichier << "Jour;Population herbivores;Population carnivores\n";
	fichier << "0;" << populationHerbivoreInit << ";" << populationCarnivoreInit << ";\n";

	calculerEvolutionPopulations(populationHerbivoreInit, populationCarnivoreInit, alpha, beta, gamma, delta, dt, 500);

	fichier.close();
}

