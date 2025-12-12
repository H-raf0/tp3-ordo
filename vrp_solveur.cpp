#include "vrp_solveur.h"

// ==================== LECTURE INSTANCE ====================

void lireInstance(std::string& filename, t_problem& pb) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Erreur pendant la lecture du fichier : " << filename << std::endl;
        return;
    }
    
    // Initialisation des quantités à 0
    for (int i = 0; i < n_max; i++) {
        pb.quantite[i] = 0;
    }
    
    // Lecture nombre de villes, camions et capacité
    file >> pb.nombresVilles;
    file >> pb.nombresCamions >> pb.capacite;
    
    // Lecture de la matrice des distances
    for (int i = 0; i < pb.nombresVilles + 1; i++) {
        for (int j = 0; j < pb.nombresVilles + 1; j++) {
            file >> pb.distance[i][j];
        }
    }
    
    // Lecture des quantités
    for (int i = 0; i < pb.nombresVilles + 1; i++) {
        int idx = 0;
        file >> idx;
        file >> pb.quantite[idx];
    }
    
    file.close();
}

void afficherInstance(t_problem& pb) {
    std::cout << "========== Instance VRP ==========" << std::endl;
    std::cout << "Nombres de villes : " << pb.nombresVilles << std::endl;
    std::cout << "Nombres de camions : " << pb.nombresCamions << std::endl;
    std::cout << "Capacite des camions : " << pb.capacite << std::endl;

    std::cout << "\nMatrice des distances :" << std::endl;
    for (int i = 0; i < std::min(5, pb.nombresVilles + 1); i++) {
        for (int j = 0; j < std::min(5, pb.nombresVilles + 1); j++) {
            std::cout << pb.distance[i][j] << " ";
        }
        std::cout << std::endl;
    }

    std::cout << "\nQuantites par sommet :" << std::endl;
    for (int i = 0; i < std::min(10, pb.nombresVilles + 1); i++) {
        std::cout << "Ville " << i << " : " << pb.quantite[i] << std::endl;
    }
}

// ==================== HEURISTIQUES CONSTRUCTIVES ====================

void plusProcheVoisin(t_problem& pb, t_solution& sol) {
    bool visite[n_max] = { false };
    int courant = 0;
    visite[0] = true;
    sol.vecteur[0] = 0;
    int pos = 1;

    // Construction du TSP géant
    for (int k = 0; k < pb.nombresVilles; k++) {
        int best = -1;
        int bestCost = 1000000000;

        // Trouver le client le plus proche non visité
        for (int j = 1; j <= pb.nombresVilles; j++) {
            if (!visite[j] && pb.distance[courant][j] < bestCost) {
                best = j;
                bestCost = pb.distance[courant][j];
            }
        }

        if (best == -1) break;

        visite[best] = true;
        sol.vecteur[pos++] = best;
        courant = best;
    }

    // Retour au dépôt
    sol.vecteur[pos++] = 0;

    // Décomposition en tournées avec SPLIT
    split(pb, sol);
}

void plusProcheVoisinRandomise(t_problem& pb, t_solution& sol, int nombresVoisins) {
    bool visite[n_max] = { false };
    int courant = 0;
    visite[0] = true;
    sol.vecteur[0] = 0;
    int pos = 1;

    for (int k = 0; k < pb.nombresVilles; k++) {
        std::vector<std::pair<int, int>> voisins; // (distance, ville)

        // Collecter tous les voisins non visités
        for (int j = 1; j <= pb.nombresVilles; j++) {
            if (!visite[j]) {
                voisins.push_back({ pb.distance[courant][j], j });
            }
        }

        // Trier par distance croissante
        std::sort(voisins.begin(), voisins.end());

        // Choisir aléatoirement parmi les N plus proches
        int nbCandidats = std::min(nombresVoisins, (int)voisins.size());
        if (nbCandidats == 0) break;
        
        int r = rand() % nbCandidats;
        int best = voisins[r].second;

        visite[best] = true;
        sol.vecteur[pos++] = best;
        courant = best;
    }

    sol.vecteur[pos++] = 0;
    
    split(pb, sol);
}

void solutionHeuristique(t_problem& pb, t_solution& sol) {
    bool visite[n_max] = { false };
    int courant = 0;
    visite[0] = true;
    sol.vecteur[0] = 0;
    int pos = 1;

    for (int k = 0; k < pb.nombresVilles; k++) {
        int best = -1;

        // Calcul de la distance moyenne vers les non visités
        float avg = 0;
        int count = 0;
        for (int j = 1; j <= pb.nombresVilles; j++) {
            if (!visite[j]) {
                avg += pb.distance[courant][j];
                count++;
            }
        }
        if (count > 0) avg = avg / count;

        // Choisir le client dont la distance est la plus proche de la moyenne
        int dif = 1000000000;
        for (int j = 1; j <= pb.nombresVilles; j++) {
            if (!visite[j] && abs(pb.distance[courant][j] - (int)avg) < dif) {
                dif = abs(pb.distance[courant][j] - (int)avg);
                best = j;
            }
        }

        if (best == -1) break;

        visite[best] = true;
        sol.vecteur[pos++] = best;
        courant = best;
    }

    sol.vecteur[pos++] = 0;
    
    split(pb, sol);
}

// ==================== TRANSFORMATION TSP -> VRP ====================

void split(t_problem& pb, t_solution& sol) {
    sol.nombresTournees = 0;
    sol.cout = 0;

    t_tournee tournee_courante;
    int pos_tournee = 0;
    tournee_courante.liste[pos_tournee++] = 0;  // Départ du dépôt
    tournee_courante.volume = 0;
    tournee_courante.cout = 0;

    // Parcourir le vecteur TSP (sans le dernier 0)
    for (int i = 1; sol.vecteur[i] != 0; i++) {
        int ville = sol.vecteur[i];
        int quantite_ville = pb.quantite[ville];

        // Vérifier si l'ajout de ce client dépasse la capacité
        if (tournee_courante.volume + quantite_ville > pb.capacite) {
            // Fermer la tournée actuelle
            int ville_precedente = tournee_courante.liste[pos_tournee - 1];
            tournee_courante.cout += pb.distance[ville_precedente][0];
            tournee_courante.liste[pos_tournee] = 0;  // Retour au dépôt
            
            // Sauvegarder cette tournée
            sol.liste_tournee[sol.nombresTournees] = tournee_courante;
            sol.nombresTournees++;

            // Créer une nouvelle tournée
            pos_tournee = 0;
            tournee_courante.liste[pos_tournee++] = 0;
            tournee_courante.volume = 0;
            tournee_courante.cout = 0;
        }

        // Ajouter le client à la tournée courante
        int ville_precedente = tournee_courante.liste[pos_tournee - 1];
        tournee_courante.cout += pb.distance[ville_precedente][ville];
        tournee_courante.liste[pos_tournee++] = ville;
        tournee_courante.volume += quantite_ville;
    }

    // Fermer la dernière tournée
    if (pos_tournee > 1) {  // Si la tournée contient au moins un client
        int ville_precedente = tournee_courante.liste[pos_tournee - 1];
        tournee_courante.cout += pb.distance[ville_precedente][0];
        tournee_courante.liste[pos_tournee] = 0;
        
        sol.liste_tournee[sol.nombresTournees] = tournee_courante;
        sol.nombresTournees++;
    }

    // Calculer le coût total
    sol.cout = 0;
    for (int i = 0; i < sol.nombresTournees; i++) {
        sol.cout += sol.liste_tournee[i].cout;
    }
}

// ==================== RECHERCHE LOCALE ====================

void rechercheLocaleDeplacement(t_problem& pb, t_solution& sol, int maxIterations) {
    plusProcheVoisin(pb, sol);
    bool amelioration = true;
    int it = 0;

    while (amelioration && it < maxIterations) {
        amelioration = false;

        for (int i = 1; i < pb.nombresVilles + 1; i++) {
            for (int j = i + 1; j < pb.nombresVilles + 1; j++) {
                // Créer une solution voisine en échangeant i et j
                t_solution solVoisin;
                for (int k = 0; k <= pb.nombresVilles + 1; k++) {
                    solVoisin.vecteur[k] = sol.vecteur[k];
                }

                // Échange
                int temp = solVoisin.vecteur[i];
                solVoisin.vecteur[i] = solVoisin.vecteur[j];
                solVoisin.vecteur[j] = temp;

                split(pb, solVoisin);

                if (solVoisin.cout < sol.cout) {
                    sol = solVoisin;
                    amelioration = true;
                }
            }
        }
        it++;
    }
}

void rechercheLocale2OPT(t_problem& pb, t_solution& sol, int maxIterations) {
    plusProcheVoisin(pb, sol);
    bool amelioration = true;
    int it = 0;

    while (amelioration && it < maxIterations) {
        amelioration = false;

        for (int i = 1; i < pb.nombresVilles; i++) {
            for (int j = i + 2; j < pb.nombresVilles + 1; j++) {
                // Créer une solution voisine
                t_solution solVoisin;
                for (int k = 0; k <= pb.nombresVilles + 1; k++) {
                    solVoisin.vecteur[k] = sol.vecteur[k];
                }

                // Inverser le segment entre i et j
                int left = i;
                int right = j;
                while (left < right) {
                    int temp = solVoisin.vecteur[left];
                    solVoisin.vecteur[left] = solVoisin.vecteur[right];
                    solVoisin.vecteur[right] = temp;
                    left++;
                    right--;
                }

                split(pb, solVoisin);

                if (solVoisin.cout < sol.cout) {
                    sol = solVoisin;
                    amelioration = true;
                }
            }
        }
        it++;
    }
}

// ==================== METAHEURISTIQUE ====================

void GRASP(t_problem& pb, t_solution& sol, int nbIterations, int maxIterations2OPT) {
    t_solution meilleureGlobale;
    meilleureGlobale.cout = 1000000000;

    for (int iteration = 0; iteration < nbIterations; iteration++) {
        t_solution solCourante;

        // Phase de construction randomisée
        plusProcheVoisinRandomise(pb, solCourante, 5);

        // Phase d'amélioration locale (2-OPT intégré)
        bool amelioration = true;
        int it = 0;

        while (amelioration && it < maxIterations2OPT) {
            amelioration = false;

            for (int i = 1; i < pb.nombresVilles; i++) {
                for (int j = i + 2; j < pb.nombresVilles + 1; j++) {
                    t_solution solVoisin;
                    for (int k = 0; k <= pb.nombresVilles + 1; k++) {
                        solVoisin.vecteur[k] = solCourante.vecteur[k];
                    }

                    int left = i;
                    int right = j;
                    while (left < right) {
                        int temp = solVoisin.vecteur[left];
                        solVoisin.vecteur[left] = solVoisin.vecteur[right];
                        solVoisin.vecteur[right] = temp;
                        left++;
                        right--;
                    }

                    split(pb, solVoisin);

                    if (solVoisin.cout < solCourante.cout) {
                        solCourante = solVoisin;
                        amelioration = true;
                    }
                }
            }
            it++;
        }

        // Mise à jour de la meilleure solution
        if (solCourante.cout < meilleureGlobale.cout) {
            meilleureGlobale = solCourante;
            std::cout << "GRASP iteration " << (iteration + 1) 
                      << " : nouvelle meilleure solution = " 
                      << meilleureGlobale.cout 
                      << " (" << meilleureGlobale.nombresTournees << " tournees)"
                      << std::endl;
        }
    }

    sol = meilleureGlobale;
}

// ==================== AFFICHAGE ====================

void afficherSolution(t_problem& pb, t_solution& sol, bool tournees) {
    std::cout << "\n========== Solution VRP ==========" << std::endl;
    std::cout << "Cout total : " << sol.cout << std::endl;
    std::cout << "Nombre de tournees : " << sol.nombresTournees << std::endl;
    
    std::cout << "\nVecteur des sommets : ";
    for (int i = 0; sol.vecteur[i] != 0 || i == 0; i++) {
        std::cout << sol.vecteur[i] << " ";
        if (sol.vecteur[i] == 0 && i > 0) break;
    }
    std::cout << std::endl;

    if (tournees) {
        std::cout << "\n=== Details des tournees ===" << std::endl;
        int volumeTotal = 0;
        for (int i = 0; i < sol.nombresTournees; i++) {
            std::cout << "\n- Tournee " << (i + 1) << " :" << std::endl;
            std::cout << "  Cout de la tournee : " << sol.liste_tournee[i].cout << std::endl;
            std::cout << "  Volume de la tournee : " << sol.liste_tournee[i].volume 
                      << "/" << pb.capacite 
                      << " (" << (100.0 * sol.liste_tournee[i].volume / pb.capacite) << "%)"
                      << std::endl;
            volumeTotal += sol.liste_tournee[i].volume;
            
            std::cout << "  Villes : ";
            int j = 0;
            do {
                std::cout << sol.liste_tournee[i].liste[j] << " ";
                j++;
            } while (sol.liste_tournee[i].liste[j] != 0);
            std::cout << sol.liste_tournee[i].liste[j] << std::endl;
        }
        std::cout << "\nVolume total collecte : " << volumeTotal << std::endl;
    }
    std::cout << "\n";
}