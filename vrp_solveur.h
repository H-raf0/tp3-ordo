#pragma once
#include <string>
#include <vector>
#include <limits>

// ==================== STRUCTURES DE DONNÉES ====================

struct Probleme {
    std::vector<int> quantites;                    // Demande de chaque client
    std::vector<std::vector<int>> distances;       // Matrice des distances
    int capacite_vehicule;                         // Capacité max d'un véhicule
    int nb_vehicules_max;                          // Nombre max de véhicules
};

struct Solution {
    std::vector<int> ordre_visite;                 // Ordre TSP : [0, clients..., 0]
    int nb_tournees;                               // Nombre de tournées
    float cout_total;                              // Coût total de la solution
    std::vector<std::vector<int>> tournees;        // Liste des tournées VRP
};

// ==================== LECTURE INSTANCE ====================

void lire_instance(const std::string& fichier, Probleme& pb);

// ==================== HEURISTIQUES CONSTRUCTIVES TSP ====================

// Plus proche voisin déterministe
Solution heuristique_plus_proche_voisin(const Probleme& pb);

// Plus proche voisin randomisé (parmi les 3 plus proches)
Solution heuristique_plus_proche_voisin_aleatoire(const Probleme& pb, double p = 0.8);

// Plus loin voisin randomisé (parmi les 3 plus éloignés)
Solution heuristique_plus_loin_voisin_aleatoire(const Probleme& pb, double p = 0.8);

// ==================== TRANSFORMATION TSP → VRP ====================

// Split illimité (autant de véhicules que nécessaire)
Solution split_illimite(const Probleme& pb, const Solution& sol_tsp);

// Split limité (respecte nb_vehicules_max)
Solution split_limite(const Probleme& pb, const Solution& sol_tsp);

// ==================== RECHERCHE LOCALE ====================

// Amélioration 2-opt intra-tournée
void amelioration_2opt(Solution& sol, const Probleme& pb);

// Déplacement de clients entre tournées
void amelioration_deplacement_clients(Solution& sol, const Probleme& pb);

// ==================== METAHEURISTIQUE ====================

// GRASP (Greedy Randomized Adaptive Search Procedure)
Solution grasp(const Probleme& pb, int nb_iterations = 50);

// ==================== UTILITAIRES ====================

// Calcul du coût total d'une solution
float calculer_cout_solution(const Solution& sol, const Probleme& pb);

// Affichage d'une solution
void afficher_solution(const Solution& sol);