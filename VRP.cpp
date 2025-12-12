#include <iostream>
#include "instance.h"
#include "plusprochevoisinstandard.h"
#include "plusprocheVoisinRandomisee.h"
#include "LoinVoisinHeuristique.h"
#include "evaluer.h"        // contient split()
#include "splitLimite.h"  // flotte limitée
#include "recherche_local.h"
#include "grasp.h"

void afficherSolution(const T_Sol& sol) {
    std::cout << "\n--- Solution VRP ---\n";
    std::cout << "Nombre de tournees : " << sol.nb_tournee << "\n";
    std::cout << "Cout total         : " << sol.cout_total_sol << "\n";

    for (int i = 0; i < sol.listeTournee.size(); i++) {
        std::cout << "Camion " << i + 1 << " : ";
        for (int v : sol.listeTournee[i]) std::cout << v << " ";
        std::cout << "\n";
    }
}

int main() {

    I_Pb pb;
    lire_instance("HVRP_DLP_05.txt", pb);

    std::cout << "\n==================== INSTANCE CHARGEE ====================\n";
    std::cout << "Capacite véhicule  : " << pb.capacite << "\n";
    std::cout << "Nb vehicules max    : " << pb.nb_vehicules << "\n";
    std::cout << "Nb sommets (avec depot) : " << pb.qte.size() << "\n";

    // 1) Construction TSP avec 3 heuristiques

    T_Sol tsp1 = plus_proche_voisins(pb);
    T_Sol tsp2 = plus_proche_voisins_random(pb);
    T_Sol tsp3 = plus_loin_voisin_random(pb);

    std::cout << "\n=== Cout TSP ===\n";
    std::cout << "Plus proche voisin        : " << tsp1.cout_total_sol << "\n";
    std::cout << "Plus proche voisin random : " << tsp2.cout_total_sol << "\n";
    std::cout << "Plus loin voisin random   : " << tsp3.cout_total_sol << "\n";

    // 2) Transformation VRP (Split) - Illimité et Limité

    T_Sol v1 = split(pb, tsp1);
    T_Sol v2 = split(pb, tsp2);
    T_Sol v3 = split(pb, tsp3);

    std::cout << "\n=== VRP Split illimite ===";
    afficherSolution(v1);

    // Version limitée (contrainte nb véhicule)

    T_Sol v1_lim = split_limite(pb, tsp1);

    std::cout << "\n=== VRP Split Limité ===";
    if (v1_lim.cout_total_sol == 1e9)
        std::cout << "\n⚠ Impossible avec " << pb.nb_vehicules << " véhicules.\n";
    else
        afficherSolution(v1_lim);


    // 3) Recherche locale

    std::cout << "\n=== Recherche locale (2-opt + deplacement) ===\n";

    deuxOpt(v1, pb);
    deplacerClient(v1, pb);

    afficherSolution(v1);

    // 4) GRASP 

    std::cout << "\n=== GRASP ===\n";
    T_Sol grasp_best = grasp_vrp(pb, 10); 

    afficherSolution(grasp_best);

    std::cout << "\n==================== FIN ====================\n";
    return 0;
}
