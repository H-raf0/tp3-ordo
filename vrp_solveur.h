#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <ctime>

const int n_max = 200;

struct t_problem {
    int nombresVilles;           // Nombre de clients (sans le dépôt)
    int quantite[n_max];         // Demande de chaque sommet
    int distance[n_max][n_max];  // Matrice des distances
    int capacite;                // Capacité des véhicules
    int nombresCamions;          // Nombre maximum de camions
};

struct t_tournee {
    int cout;              // Coût de la tournée
    int liste[n_max];      // Liste des sommets visités
    int volume;            // Charge totale de la tournée
};

struct t_solution {
    int vecteur[n_max];              // Ordre de visite TSP
    int nombresTournees;             // Nombre de tournées
    int cout;                        // Coût total
    t_tournee liste_tournee[n_max]; // Liste des tournées
};

// ==================== LECTURE INSTANCE ====================
void lireInstance(std::string& filename, t_problem& pb);
void afficherInstance(t_problem& pb);

// ==================== HEURISTIQUES CONSTRUCTIVES ====================
void plusProcheVoisin(t_problem& pb, t_solution& sol);
void plusProcheVoisinRandomise(t_problem& pb, t_solution& sol, int nombresVoisins);
void solutionHeuristique(t_problem& pb, t_solution& sol);

// ==================== TRANSFORMATION TSP -> VRP ====================
void split(t_problem& pb, t_solution& sol);

// ==================== RECHERCHE LOCALE ====================
void rechercheLocaleDeplacement(t_problem& pb, t_solution& sol, int maxIterations);
void rechercheLocale2OPT(t_problem& pb, t_solution& sol, int maxIterations);

// ==================== METAHEURISTIQUE ====================
void GRASP(t_problem& pb, t_solution& sol, int nbIterations, int maxIterations2OPT);

// ==================== AFFICHAGE ====================
void afficherSolution(t_problem& pb, t_solution& sol, bool tournees = true);