#include "recherche_local.h"
#include <algorithm>
#include <vector>

// Coût total 
float cout_solution(const T_Sol& sol, const I_Pb& pb) {
    float total = 0.0f;
    for (const auto& T : sol.listeTournee) {
        if (T.size() < 2) continue;
        for (size_t k = 0; k + 1 < T.size(); ++k) {
            int a = T[k];
            int b = T[k + 1];
            total += pb.dist[a][b];
        }
    }
    return total;
}

// 2-OPT 
void deuxOpt(T_Sol& solVRP, const I_Pb& pb) {

    for (auto& tour : solVRP.listeTournee) {
        if (tour.size() < 4) continue;

        bool improved = true;

        while (improved) {
            improved = false;

            for (int i = 1; i < (int)tour.size() - 2; ++i) {
                for (int j = i + 1; j < (int)tour.size() - 1; ++j) {

                    int a = tour[i - 1];
                    int b = tour[i];
                    int c = tour[j];
                    int d = tour[j + 1];

                    int before = pb.dist[a][b] + pb.dist[c][d];
                    int after = pb.dist[a][c] + pb.dist[b][d];

                    if (after < before) {
                        std::reverse(tour.begin() + i, tour.begin() + j + 1);
                        improved = true;
                    }
                }
            }
        }
    }

    solVRP.cout_total_sol = cout_solution(solVRP, pb);
}


//déplacer client
void deplacerClient(T_Sol& solVRP, const I_Pb& pb) {

    bool improved = true;

    while (improved) {
        improved = false;

        int nbR = solVRP.listeTournee.size();

        for (int r1 = 0; r1 < nbR; r1++) {

            auto& R1 = solVRP.listeTournee[r1];

            // au moins 0 - x - 0
            if (R1.size() <= 3) continue;

            for (int i = 1; i < (int)R1.size() - 1; i++) {

                int client = R1[i];

                for (int r2 = 0; r2 < nbR; r2++) {
                    if (r1 == r2) continue;

                    auto& R2 = solVRP.listeTournee[r2];

                    // éviter tournée inutile ou size==2 → 0-0
                    if (R2.size() < 2) continue;

                    for (int j = 1; j <= (int)R2.size() - 1; j++) {

                        // ------- copies TEMPORAIRES pour tester move ------
                        std::vector<int> T1 = R1;
                        std::vector<int> T2 = R2;

                        T1.erase(T1.begin() + i);     // remove from R1
                        T2.insert(T2.begin() + j, client); // add to R2

                        // Safety : T1 doit minimum être 0-x-0
                        if (T1.size() < 3) continue;

                        // ------- Vérification capacité -------
                        int q1 = 0, q2 = 0;
                        for (int v : T1) q1 += pb.qte[v];
                        for (int v : T2) q2 += pb.qte[v];
                        if (q1 > pb.capacite || q2 > pb.capacite) continue;

                        // ------- calcul du coût des deux tournées -------
                        auto calc = [&](const std::vector<int>& t) {
                            int cst = 0;
                            for (int k = 0; k + 1 < t.size(); k++)
                                cst += pb.dist[t[k]][t[k + 1]];
                            return cst;
                            };

                        int oldCost = calc(R1) + calc(R2);
                        int newCost = calc(T1) + calc(T2);

                        // ------- amélioration trouvée -> on valide move -------
                        if (newCost < oldCost) {
                            R1 = T1;
                            R2 = T2;
                            solVRP.cout_total_sol = cout_solution(solVRP, pb);
                            improved = true;
                            goto RESTART_SEARCH; // relancer proprement
                        }
                    }
                }
            }
        }
    RESTART_SEARCH:
        continue;
    }
}

