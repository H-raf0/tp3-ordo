#include "instance.h"
#include <vector>
#include <limits>
#include <algorithm>
#include <iostream>

// ============= Version flotte LIMITÉE ============= //

T_Sol split_limite(const I_Pb& pb, const T_Sol& solTSP) {

    const std::vector<int>& TG = solTSP.clientsVisites;
    int n = TG.size() - 2; // car TG = [0 ... clients ... 0]

    std::vector<float> m(n + 1, std::numeric_limits<float>::infinity());
    std::vector<int> pere(n + 1, -1);
    m[0] = 0;

    std::vector<std::vector<float>> cout(n + 1, std::vector<float>(n + 1, -1));

    // ----------------- Pré-calcul des coûts de sous-tournées ----------------- //
    for (int i = 0; i < n; i++) {

        int quantite = 0;
        float coutTournee = 0;

        for (int j = i + 1; j <= n; j++) {

            int client = TG[j];
            int q = pb.qte[client];

            if (quantite + q > pb.capacite)
                break; // dépasse capacité → stop ici

            quantite += q;

            if (j == i + 1) {
                coutTournee = pb.dist[TG[i]][TG[j]] + pb.dist[TG[j]][TG[i]];
            }
            else {
                coutTournee =
                    coutTournee
                    - pb.dist[TG[j - 1]][TG[i]]
                    + pb.dist[TG[j - 1]][TG[j]]
                    + pb.dist[TG[j]][TG[i]];
            }

            cout[i][j] = coutTournee;
        }
    }

    // ----------------- Programmation dynamique ----------------- //
    for (int i = 1; i <= n; i++) {
        for (int j = 0; j < i; j++) {

            if (cout[j][i] >= 0) {
                float c = m[j] + cout[j][i];

                if (c < m[i]) {
                    m[i] = c;
                    pere[i] = j;
                }
            }
        }
    }

    // ----------------- Reconstruction solution ----------------- //
    T_Sol solVRP;
    solVRP.clientsVisites = TG;
    solVRP.listeTournee.clear();
    solVRP.nb_tournee = 0;

    int i = n;

    while (i > 0) {
        int j = pere[i];

        std::vector<int> tournee = { 0 };

        for (int k = j + 1; k <= i; k++)
            tournee.push_back(TG[k]);

        tournee.push_back(0);

        solVRP.listeTournee.push_back(tournee);
        solVRP.nb_tournee++;

        // --------- CONTRÔLE FLOTTE LIMITÉE --------- //
        if (solVRP.nb_tournee > pb.nb_vehicules) {
            solVRP.cout_total_sol = 1e9; // solution invalide
            return solVRP;
        }

        i = j;
    }

    std::reverse(solVRP.listeTournee.begin(), solVRP.listeTournee.end());

    float total = 0;
    for (auto& T : solVRP.listeTournee)
        for (int k = 0; k < T.size() - 1; k++)
            total += pb.dist[T[k]][T[k + 1]];

    solVRP.cout_total_sol = total;
    return solVRP;
}
