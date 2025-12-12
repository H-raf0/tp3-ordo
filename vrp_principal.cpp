#include "vrp_solveur.h"

int main() {
    // Initialisation du générateur aléatoire
    srand(time(NULL));
    
    t_problem pb;
    t_solution sol;
    
    // Nom du fichier d'instance
    std::string file = "HVRP_DLP_05.txt";
    
    std::cout << "============================================================" << std::endl;
    std::cout << "          SOLVEUR VRP - CHARGEMENT DE L'INSTANCE           " << std::endl;
    std::cout << "============================================================" << std::endl;
    
    // Lecture de l'instance
    lireInstance(file, pb);
    
    std::cout << "\nInstance chargee : " << file << std::endl;
    std::cout << "Nombre de clients : " << pb.nombresVilles << std::endl;
    std::cout << "Nombre de camions max : " << pb.nombresCamions << std::endl;
    std::cout << "Capacite des camions : " << pb.capacite << std::endl;
    
    // ==================== TESTS DES HEURISTIQUES ====================
    
    std::cout << "\n\n============================================================" << std::endl;
    std::cout << "              PHASE 1 : HEURISTIQUES CONSTRUCTIVES         " << std::endl;
    std::cout << "============================================================" << std::endl;
    
    // Test 1 : Plus proche voisin
    std::cout << "\n[1] Plus proche voisin deterministe" << std::endl;
    t_solution sol1;
    plusProcheVoisin(pb, sol1);
    std::cout << "Cout total : " << sol1.cout << std::endl;
    std::cout << "Nombre de tournees : " << sol1.nombresTournees << std::endl;
    
    // Test 2 : Plus proche voisin randomisé
    std::cout << "\n[2] Plus proche voisin randomise (3 voisins)" << std::endl;
    t_solution sol2;
    plusProcheVoisinRandomise(pb, sol2, 3);
    std::cout << "Cout total : " << sol2.cout << std::endl;
    std::cout << "Nombre de tournees : " << sol2.nombresTournees << std::endl;
    
    // Test 3 : Heuristique moyenne
    std::cout << "\n[3] Heuristique distance moyenne" << std::endl;
    t_solution sol3;
    solutionHeuristique(pb, sol3);
    std::cout << "Cout total : " << sol3.cout << std::endl;
    std::cout << "Nombre de tournees : " << sol3.nombresTournees << std::endl;
    
    // ==================== RECHERCHE LOCALE ====================
    
    std::cout << "\n\n============================================================" << std::endl;
    std::cout << "              PHASE 2 : RECHERCHE LOCALE                    " << std::endl;
    std::cout << "============================================================" << std::endl;
    
    std::cout << "\n[4] Recherche locale 2-OPT (100 iterations)" << std::endl;
    t_solution sol4;
    rechercheLocale2OPT(pb, sol4, 100);
    std::cout << "Cout final : " << sol4.cout << std::endl;
    std::cout << "Nombre de tournees : " << sol4.nombresTournees << std::endl;
    
    std::cout << "\n[5] Recherche locale deplacement (100 iterations)" << std::endl;
    t_solution sol5;
    rechercheLocaleDeplacement(pb, sol5, 100);
    std::cout << "Cout final : " << sol5.cout << std::endl;
    std::cout << "Nombre de tournees : " << sol5.nombresTournees << std::endl;
    
    // ==================== GRASP ====================
    
    std::cout << "\n\n============================================================" << std::endl;
    std::cout << "              PHASE 3 : METAHEURISTIQUE GRASP              " << std::endl;
    std::cout << "============================================================" << std::endl;
    
    std::cout << "\n[6] GRASP (50 iterations, 2-OPT avec 100 iterations)" << std::endl;
    t_solution solGRASP;
    GRASP(pb, solGRASP, 50, 100);
    
    // ==================== AFFICHAGE SOLUTION FINALE ====================
    
    std::cout << "\n\n============================================================" << std::endl;
    std::cout << "              SOLUTION FINALE (GRASP)                      " << std::endl;
    std::cout << "============================================================" << std::endl;
    
    afficherSolution(pb, solGRASP, true);
    
    // ==================== COMPARAISON ====================
    
    std::cout << "\n============================================================" << std::endl;
    std::cout << "              COMPARAISON DES METHODES                     " << std::endl;
    std::cout << "============================================================" << std::endl;
    std::cout << "Plus proche voisin         : " << sol1.cout << std::endl;
    std::cout << "Plus proche voisin random  : " << sol2.cout << std::endl;
    std::cout << "Heuristique moyenne        : " << sol3.cout << std::endl;
    std::cout << "Recherche locale 2-OPT     : " << sol4.cout << std::endl;
    std::cout << "Recherche locale deplacement: " << sol5.cout << std::endl;
    std::cout << "GRASP (meilleure)          : " << solGRASP.cout << std::endl;
    std::cout << "============================================================" << std::endl;
    
    std::cout << "\n*** TERMINE ***\n" << std::endl;
    
    return 0;
}