#include <iostream>
#include <cstdlib>
#include <ctime>
#include "vrp_solveur.h"

int main() {

    // IMPORTANT : Initialiser le générateur aléatoire UNE SEULE FOIS
    std::srand((unsigned)std::time(nullptr));

    // ==================== CHARGEMENT DE L'INSTANCE ====================
    
    Probleme pb;
    lire_instance("HVRP_DLP_05.txt", pb);

    std::cout << "\n";
    std::cout << "============================================================\n";
    std::cout << "          SOLVEUR VRP - INSTANCE CHARGEE                   \n";
    std::cout << "============================================================\n";
    std::cout << " Capacite vehicule   : " << pb.capacite_vehicule << " unites\n";
    std::cout << " Nb vehicules max    : " << pb.nb_vehicules_max << " camions\n";
    std::cout << " Nb clients (+ depot): " << pb.distances.size() << " sommets\n";
    std::cout << "============================================================\n";

    // ==================== PHASE 1 : CONSTRUCTION TSP ====================
    
    std::cout << "\n\n-----------------------------------------\n";
    std::cout << " PHASE 1 : CONSTRUCTION TSP (3 methodes) \n";
    std::cout << "-----------------------------------------\n\n";

    std::cout << "[1] Heuristique 1 : Plus proche voisin deterministe...\n";
    Solution tsp1 = heuristique_plus_proche_voisin(pb);
    std::cout << "    => Cout TSP : " << tsp1.cout_total << "\n\n";

    std::cout << "[2] Heuristique 2 : Plus proche voisin randomise...\n";
    Solution tsp2 = heuristique_plus_proche_voisin_aleatoire(pb, 0.8);
    std::cout << "    => Cout TSP : " << tsp2.cout_total << "\n\n";

    std::cout << "[3] Heuristique 3 : Plus loin voisin randomise...\n";
    Solution tsp3 = heuristique_plus_loin_voisin_aleatoire(pb, 0.8);
    std::cout << "    => Cout TSP : " << tsp3.cout_total << "\n\n";

    // ==================== PHASE 2 : TRANSFORMATION VRP (SPLIT) ====================
    
    std::cout << "\n----------------------------------------------\n";
    std::cout << " PHASE 2 : TRANSFORMATION TSP -> VRP (SPLIT)  \n";
    std::cout << "----------------------------------------------\n";

    std::cout << "\n[SPLIT ILLIMITE] Flotte non contrainte...\n";
    Solution vrp1 = split_illimite(pb, tsp1);
    afficher_solution(vrp1);

    std::cout << "\n[SPLIT LIMITE] Contrainte de flotte...\n";
    Solution vrp1_limite = split_limite(pb, tsp1);
    
    if (vrp1_limite.cout_total == 1e9) {
        std::cout << "\n*** IMPOSSIBLE de respecter la limite de " 
                  << pb.nb_vehicules_max << " vehicules ! ***\n\n";
    } else {
        afficher_solution(vrp1_limite);
    }

    // ==================== PHASE 3 : RECHERCHE LOCALE ====================
    
    std::cout << "\n---------------------------------------\n";
    std::cout << " PHASE 3 : RECHERCHE LOCALE (2-OPT)   \n";
    std::cout << "---------------------------------------\n\n";

    std::cout << "[AMELIORATION 2-OPT] Intra-tournee...\n";
    float cout_avant_2opt = vrp1.cout_total;
    amelioration_2opt(vrp1, pb);
    std::cout << "    Cout avant : " << cout_avant_2opt << "\n";
    std::cout << "    Cout apres : " << vrp1.cout_total << "\n";
    std::cout << "    Gain       : " << (cout_avant_2opt - vrp1.cout_total) << "\n\n";

    std::cout << "[DEPLACEMENT] Clients entre tournees...\n";
    float cout_avant_deplacement = vrp1.cout_total;
    amelioration_deplacement_clients(vrp1, pb);
    std::cout << "    Cout avant : " << cout_avant_deplacement << "\n";
    std::cout << "    Cout apres : " << vrp1.cout_total << "\n";
    std::cout << "    Gain       : " << (cout_avant_deplacement - vrp1.cout_total) << "\n\n";

    afficher_solution(vrp1);

    // ==================== PHASE 4 : METAHEURISTIQUE GRASP ====================
    
    std::cout << "\n------------------------------------------------\n";
    std::cout << " PHASE 4 : METAHEURISTIQUE GRASP (50 iterations)\n";
    std::cout << "------------------------------------------------\n\n";

    std::cout << "[GRASP] Lancement de l'optimisation...\n";
    Solution meilleure_grasp = grasp(pb, 50);
    
    std::cout << "\n*** MEILLEURE SOLUTION TROUVEE PAR GRASP ***\n";
    afficher_solution(meilleure_grasp);

    // ==================== COMPARAISON FINALE ====================
    
    std::cout << "\n------------------------------------------\n";
    std::cout << " COMPARAISON DES METHODES                 \n";
    std::cout << "------------------------------------------\n";
    std::cout << " Plus proche voisin + Split : " << split_illimite(pb, tsp1).cout_total << "\n";
    std::cout << " + Recherche locale         : " << vrp1.cout_total << "\n";
    std::cout << " GRASP (50 iterations)      : " << meilleure_grasp.cout_total << "\n";
    std::cout << "------------------------------------------\n";

    std::cout << "\n*** TERMINE ***\n\n";

    return 0;
}