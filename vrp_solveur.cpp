#include "vrp_solveur.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <ctime>

// ==================== LECTURE INSTANCE ====================

void lire_instance(const std::string& fichier, Probleme& pb) {
    std::ifstream f(fichier);
    if (!f) {
        std::cerr << "ERREUR : impossible d'ouvrir " << fichier << std::endl;
        return;
    }

    int n, depot;
    f >> n;  // Nombre de clients (sans le dépôt)
    int total = n + 1;  // Total = clients + dépôt

    f >> pb.nb_vehicules_max >> pb.capacite_vehicule;
    f >> depot;  // Index du dépôt (généralement 0)

    // Lecture de la matrice des distances
    pb.distances.assign(total, std::vector<int>(total));
    for (int i = 0; i < total; i++)
        for (int j = 0; j < total; j++)
            f >> pb.distances[i][j];

    // Lecture des quantités demandées
    std::string ligne_poubelle;
    std::getline(f, ligne_poubelle);
    std::getline(f, ligne_poubelle);

    pb.quantites.assign(total, 0);
    int idx, qte;
    while (f >> idx >> qte) {
        if (idx >= 1 && idx <= n)
            pb.quantites[idx] = qte;
    }
}

// ==================== HEURISTIQUES CONSTRUCTIVES TSP ====================

Solution heuristique_plus_proche_voisin(const Probleme& pb) {
    int n = pb.distances.size();
    
    Solution sol;
    sol.ordre_visite.clear();
    sol.tournees.clear();
    sol.nb_tournees = 1;
    sol.cout_total = 0;

    // Ordre de visite (TSP géant)
    std::vector<int>& ordre = sol.ordre_visite;
    ordre.push_back(0);  // Départ du dépôt

    // Liste des clients non visités
    std::vector<int> clients_restants;
    for (int i = 1; i < n; i++)
        clients_restants.push_back(i);

    // Construction gourmande
    while (!clients_restants.empty()) {
        int courant = ordre.back();
        int plus_proche = -1;
        int dist_min = std::numeric_limits<int>::max();

        // Chercher le client le plus proche
        for (int client : clients_restants) {
            int d = pb.distances[courant][client];
            if (d < dist_min) {
                dist_min = d;
                plus_proche = client;
            }
        }

        ordre.push_back(plus_proche);
        sol.cout_total += dist_min;

        // Retirer le client de la liste
        clients_restants.erase(
            std::remove(clients_restants.begin(), clients_restants.end(), plus_proche),
            clients_restants.end()
        );
    }

    // Retour au dépôt
    sol.cout_total += pb.distances[ordre.back()][0];
    ordre.push_back(0);

    sol.tournees.push_back(ordre);
    return sol;
}

Solution heuristique_plus_proche_voisin_aleatoire(const Probleme& pb, double p) {
    int n = pb.distances.size();
    
    Solution sol;
    sol.ordre_visite.clear();
    sol.tournees.clear();
    sol.nb_tournees = 1;
    sol.cout_total = 0;

    std::vector<int>& ordre = sol.ordre_visite;
    ordre.push_back(0);

    std::vector<int> clients_restants;
    for (int i = 1; i < n; i++)
        clients_restants.push_back(i);

    while (!clients_restants.empty()) {
        int courant = ordre.back();

        // Trouver les 3 plus proches voisins
        int candidats[3] = {-1, -1, -1};
        int distances_candidats[3] = {999999999, 999999999, 999999999};
        int nb_candidats = 0;

        for (int client : clients_restants) {
            int d = pb.distances[courant][client];

            if (nb_candidats < 3) {
                candidats[nb_candidats] = client;
                distances_candidats[nb_candidats] = d;
                nb_candidats++;

                // Tri des candidats par distance croissante
                for (int i = 0; i < nb_candidats - 1; i++)
                    for (int j = i + 1; j < nb_candidats; j++)
                        if (distances_candidats[j] < distances_candidats[i]) {
                            std::swap(distances_candidats[i], distances_candidats[j]);
                            std::swap(candidats[i], candidats[j]);
                        }
            }
            else {
                // Remplacer le plus loin si d est plus petit
                int idx_max = 0;
                for (int i = 1; i < 3; i++)
                    if (distances_candidats[i] > distances_candidats[idx_max])
                        idx_max = i;

                if (d < distances_candidats[idx_max]) {
                    distances_candidats[idx_max] = d;
                    candidats[idx_max] = client;

                    // Re-trier
                    for (int i = 0; i < 2; i++)
                        for (int j = i + 1; j < 3; j++)
                            if (distances_candidats[j] < distances_candidats[i]) {
                                std::swap(distances_candidats[i], distances_candidats[j]);
                                std::swap(candidats[i], candidats[j]);
                            }
                }
            }
        }

        // Sélection aléatoire biaisée
        double u = (double)rand() / RAND_MAX;
        int choisi;

        if (u < p)
            choisi = candidats[0];  // Le plus proche avec probabilité p
        else if (u < p + (1 - p) * p)
            choisi = candidats[1];  // Le 2ème avec probabilité (1-p)*p
        else
            choisi = candidats[2];  // Le 3ème avec probabilité (1-p)²

        ordre.push_back(choisi);
        sol.cout_total += pb.distances[courant][choisi];

        clients_restants.erase(
            std::remove(clients_restants.begin(), clients_restants.end(), choisi),
            clients_restants.end()
        );
    }

    sol.cout_total += pb.distances[ordre.back()][0];
    ordre.push_back(0);
    sol.tournees.push_back(ordre);

    return sol;
}

Solution heuristique_plus_loin_voisin_aleatoire(const Probleme& pb, double p) {
    int n = pb.distances.size();
    
    Solution sol;
    sol.ordre_visite.clear();
    sol.tournees.clear();
    sol.nb_tournees = 1;
    sol.cout_total = 0;

    std::vector<int>& ordre = sol.ordre_visite;
    ordre.push_back(0);

    std::vector<int> clients_restants;
    for (int i = 1; i < n; i++)
        clients_restants.push_back(i);

    while (!clients_restants.empty()) {
        int courant = ordre.back();

        // Trouver les 3 PLUS LOINTAINS voisins
        int candidats[3] = {-1, -1, -1};
        int distances_candidats[3] = {-1, -1, -1};
        int nb_candidats = 0;

        for (int client : clients_restants) {
            int d = pb.distances[courant][client];

            if (nb_candidats < 3) {
                candidats[nb_candidats] = client;
                distances_candidats[nb_candidats] = d;
                nb_candidats++;

                // Tri par distance décroissante
                for (int i = 0; i < nb_candidats - 1; i++)
                    for (int j = i + 1; j < nb_candidats; j++)
                        if (distances_candidats[j] > distances_candidats[i]) {
                            std::swap(distances_candidats[i], distances_candidats[j]);
                            std::swap(candidats[i], candidats[j]);
                        }
            }
            else {
                int idx_min = 0;
                for (int i = 1; i < 3; i++)
                    if (distances_candidats[i] < distances_candidats[idx_min])
                        idx_min = i;

                if (d > distances_candidats[idx_min]) {
                    distances_candidats[idx_min] = d;
                    candidats[idx_min] = client;

                    for (int i = 0; i < 2; i++)
                        for (int j = i + 1; j < 3; j++)
                            if (distances_candidats[j] > distances_candidats[i]) {
                                std::swap(distances_candidats[i], distances_candidats[j]);
                                std::swap(candidats[i], candidats[j]);
                            }
                }
            }
        }

        // Sélection aléatoire biaisée
        double u = (double)rand() / RAND_MAX;
        int choisi;

        if (u < p)
            choisi = candidats[0];  // Le plus loin
        else if (u < p + (1 - p) * p)
            choisi = candidats[1];
        else
            choisi = candidats[2];

        ordre.push_back(choisi);
        sol.cout_total += pb.distances[courant][choisi];

        clients_restants.erase(
            std::remove(clients_restants.begin(), clients_restants.end(), choisi),
            clients_restants.end()
        );
    }

    sol.cout_total += pb.distances[ordre.back()][0];
    ordre.push_back(0);
    sol.tournees.push_back(ordre);

    return sol;
}

// ==================== TRANSFORMATION TSP → VRP (SPLIT) ====================

Solution split_illimite(const Probleme& pb, const Solution& sol_tsp) {
    const std::vector<int>& ordre = sol_tsp.ordre_visite;
    int n = ordre.size() - 2;  // Sans les dépôts de début et fin

    // Programmation dynamique
    std::vector<float> cout_min(n + 1, std::numeric_limits<float>::infinity());
    std::vector<int> predecesseur(n + 1, -1);
    cout_min[0] = 0;

    // Pré-calcul des coûts de sous-tournées
    std::vector<std::vector<float>> cout_tournee(n + 1, std::vector<float>(n + 1, -1));

    for (int i = 0; i < n; i++) {
        int charge = 0;
        float cout = 0;

        for (int j = i + 1; j <= n; j++) {
            int client = ordre[j];
            int qte = pb.quantites[client];

            if (charge + qte > pb.capacite_vehicule)
                break;  // Capacité dépassée

            charge += qte;

            if (j == i + 1) {
                cout = pb.distances[ordre[i]][ordre[j]] + pb.distances[ordre[j]][ordre[i]];
            }
            else {
                cout = cout - pb.distances[ordre[j-1]][ordre[i]]
                           + pb.distances[ordre[j-1]][ordre[j]]
                           + pb.distances[ordre[j]][ordre[i]];
            }

            cout_tournee[i][j] = cout;
        }
    }

    // Programmation dynamique : trouver la meilleure décomposition
    for (int i = 1; i <= n; i++) {
        for (int j = 0; j < i; j++) {
            if (cout_tournee[j][i] >= 0) {
                float c = cout_min[j] + cout_tournee[j][i];
                if (c < cout_min[i]) {
                    cout_min[i] = c;
                    predecesseur[i] = j;
                }
            }
        }
    }

    // Reconstruction de la solution VRP
    Solution sol_vrp;
    sol_vrp.ordre_visite = ordre;
    sol_vrp.tournees.clear();
    sol_vrp.nb_tournees = 0;

    int i = n;
    while (i > 0) {
        int j = predecesseur[i];
        
        std::vector<int> tournee = {0};
        for (int k = j + 1; k <= i; k++)
            tournee.push_back(ordre[k]);
        tournee.push_back(0);

        sol_vrp.tournees.push_back(tournee);
        sol_vrp.nb_tournees++;
        i = j;
    }

    std::reverse(sol_vrp.tournees.begin(), sol_vrp.tournees.end());

    sol_vrp.cout_total = calculer_cout_solution(sol_vrp, pb);
    return sol_vrp;
}

Solution split_limite(const Probleme& pb, const Solution& sol_tsp) {
    const std::vector<int>& ordre = sol_tsp.ordre_visite;
    int n = ordre.size() - 2;

    std::vector<float> cout_min(n + 1, std::numeric_limits<float>::infinity());
    std::vector<int> predecesseur(n + 1, -1);
    cout_min[0] = 0;

    std::vector<std::vector<float>> cout_tournee(n + 1, std::vector<float>(n + 1, -1));

    for (int i = 0; i < n; i++) {
        int charge = 0;
        float cout = 0;

        for (int j = i + 1; j <= n; j++) {
            int client = ordre[j];
            int qte = pb.quantites[client];

            if (charge + qte > pb.capacite_vehicule)
                break;

            charge += qte;

            if (j == i + 1) {
                cout = pb.distances[ordre[i]][ordre[j]] + pb.distances[ordre[j]][ordre[i]];
            }
            else {
                cout = cout - pb.distances[ordre[j-1]][ordre[i]]
                           + pb.distances[ordre[j-1]][ordre[j]]
                           + pb.distances[ordre[j]][ordre[i]];
            }

            cout_tournee[i][j] = cout;
        }
    }

    for (int i = 1; i <= n; i++) {
        for (int j = 0; j < i; j++) {
            if (cout_tournee[j][i] >= 0) {
                float c = cout_min[j] + cout_tournee[j][i];
                if (c < cout_min[i]) {
                    cout_min[i] = c;
                    predecesseur[i] = j;
                }
            }
        }
    }

    Solution sol_vrp;
    sol_vrp.ordre_visite = ordre;
    sol_vrp.tournees.clear();
    sol_vrp.nb_tournees = 0;

    int i = n;
    while (i > 0) {
        int j = predecesseur[i];
        
        std::vector<int> tournee = {0};
        for (int k = j + 1; k <= i; k++)
            tournee.push_back(ordre[k]);
        tournee.push_back(0);

        sol_vrp.tournees.push_back(tournee);
        sol_vrp.nb_tournees++;

        // CONTRÔLE : nombre de véhicules limité
        if (sol_vrp.nb_tournees > pb.nb_vehicules_max) {
            sol_vrp.cout_total = 1e9;  // Solution invalide
            return sol_vrp;
        }

        i = j;
    }

    std::reverse(sol_vrp.tournees.begin(), sol_vrp.tournees.end());
    sol_vrp.cout_total = calculer_cout_solution(sol_vrp, pb);
    return sol_vrp;
}

// ==================== RECHERCHE LOCALE ====================

void amelioration_2opt(Solution& sol, const Probleme& pb) {
    for (auto& tournee : sol.tournees) {
        if (tournee.size() < 4) continue;

        bool amelioration = true;
        while (amelioration) {
            amelioration = false;

            for (int i = 1; i < (int)tournee.size() - 2; i++) {
                for (int j = i + 1; j < (int)tournee.size() - 1; j++) {
                    int a = tournee[i - 1];
                    int b = tournee[i];
                    int c = tournee[j];
                    int d = tournee[j + 1];

                    int cout_avant = pb.distances[a][b] + pb.distances[c][d];
                    int cout_apres = pb.distances[a][c] + pb.distances[b][d];

                    if (cout_apres < cout_avant) {
                        std::reverse(tournee.begin() + i, tournee.begin() + j + 1);
                        amelioration = true;
                    }
                }
            }
        }
    }

    sol.cout_total = calculer_cout_solution(sol, pb);
}

void amelioration_deplacement_clients(Solution& sol, const Probleme& pb) {
    bool amelioration = true;

    while (amelioration) {
        amelioration = false;
        int nb_tournees = sol.tournees.size();

        for (int t1 = 0; t1 < nb_tournees; t1++) {
            auto& tournee1 = sol.tournees[t1];
            if (tournee1.size() <= 3) continue;

            for (int i = 1; i < (int)tournee1.size() - 1; i++) {
                int client = tournee1[i];

                for (int t2 = 0; t2 < nb_tournees; t2++) {
                    if (t1 == t2) continue;

                    auto& tournee2 = sol.tournees[t2];
                    if (tournee2.size() < 2) continue;

                    for (int j = 1; j <= (int)tournee2.size() - 1; j++) {
                        // Copies temporaires
                        std::vector<int> temp1 = tournee1;
                        std::vector<int> temp2 = tournee2;

                        temp1.erase(temp1.begin() + i);
                        temp2.insert(temp2.begin() + j, client);

                        if (temp1.size() < 3) continue;

                        // Vérification capacité
                        int charge1 = 0, charge2 = 0;
                        for (int v : temp1) charge1 += pb.quantites[v];
                        for (int v : temp2) charge2 += pb.quantites[v];

                        if (charge1 > pb.capacite_vehicule || charge2 > pb.capacite_vehicule)
                            continue;

                        // Calcul du coût
                        auto calculer_cout_tournee = [&](const std::vector<int>& t) {
                            int c = 0;
                            for (int k = 0; k + 1 < t.size(); k++)
                                c += pb.distances[t[k]][t[k + 1]];
                            return c;
                        };

                        int cout_avant = calculer_cout_tournee(tournee1) + calculer_cout_tournee(tournee2);
                        int cout_apres = calculer_cout_tournee(temp1) + calculer_cout_tournee(temp2);

                        if (cout_apres < cout_avant) {
                            tournee1 = temp1;
                            tournee2 = temp2;
                            sol.cout_total = calculer_cout_solution(sol, pb);
                            amelioration = true;
                            goto RECOMMENCER;
                        }
                    }
                }
            }
        }
    RECOMMENCER:
        continue;
    }
}

// ==================== METAHEURISTIQUE GRASP ====================

Solution grasp(const Probleme& pb, int nb_iterations) {
    Solution meilleure;
    meilleure.cout_total = 1e9f;

    int total = pb.distances.size();

    for (int iter = 0; iter < nb_iterations; iter++) {
        // Construction aléatoire
        Solution sol_tsp = heuristique_plus_proche_voisin_aleatoire(pb);

        // Vérification des indices
        bool valide = true;
        for (int v : sol_tsp.ordre_visite) {
            if (v < 0 || v >= total) {
                valide = false;
                break;
            }
        }
        if (!valide) continue;

        // Transformation VRP
        Solution sol_vrp = split_illimite(pb, sol_tsp);
        if (sol_vrp.tournees.empty()) continue;

        // Amélioration locale
        amelioration_2opt(sol_vrp, pb);

        // Mise à jour du meilleur
        if (sol_vrp.cout_total < meilleure.cout_total) {
            meilleure = sol_vrp;
        }
    }

    return meilleure;
}

// ==================== UTILITAIRES ====================

float calculer_cout_solution(const Solution& sol, const Probleme& pb) {
    float total = 0.0f;
    for (const auto& tournee : sol.tournees) {
        if (tournee.size() < 2) continue;
        for (size_t k = 0; k + 1 < tournee.size(); k++) {
            int a = tournee[k];
            int b = tournee[k + 1];
            total += pb.distances[a][b];
        }
    }
    return total;
}

void afficher_solution(const Solution& sol) {
    std::cout << "\n========================================\n";
    std::cout << "           SOLUTION VRP                 \n";
    std::cout << "========================================\n";
    std::cout << " Nombre de tournees : " << sol.nb_tournees << "\n";
    std::cout << " Cout total        : " << sol.cout_total << "\n";
    std::cout << "========================================\n\n";

    for (int i = 0; i < sol.tournees.size(); i++) {
        std::cout << "Vehicule " << (i + 1) << " : ";
        for (int client : sol.tournees[i])
            std::cout << client << " -> ";
        std::cout << "Fin\n";
    }
}