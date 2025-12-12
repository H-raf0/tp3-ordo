#include "instance.h"
#include <limits>
#include <algorithm>
#include <vector>
#include <iostream>





T_Sol split(const I_Pb& pb, const T_Sol& solTSP) {

    const std::vector<int>& TG = solTSP.clientsVisites;
    int n = TG.size() - 2; 

    std::vector<float> m(n + 1, std::numeric_limits<float>::infinity());

    std::vector<int> pere(n + 1, -1);

    m[0] = 0; 

    std::vector<std::vector<float>> cout(n + 1, std::vector<float>(n + 1, -1));

    for (int i = 0; i < n; i++) {

        int quantite = 0;

        float coutTournee = 0;

        for (int j = i + 1; j <= n; j++) {

            int client = TG[j];
            int q = pb.qte[client];

            if (quantite + q > pb.capacite)
                break;

            quantite += q;

            if (j == i + 1) {
                coutTournee =
                    pb.dist[TG[i]][TG[j]] +
                    pb.dist[TG[j]][TG[i]];
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




    //etape2
    
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


    T_Sol solVRP;
    solVRP.clientsVisites = TG;
    solVRP.listeTournee.clear();
    solVRP.nb_tournee = 0;

    int i = n;

    while (i > 0) {
        int j = pere[i];

        std::vector<int> tournee;
        tournee.push_back(0);

        for (int k = j + 1; k <= i; k++)
            tournee.push_back(TG[k]);

        tournee.push_back(0);

        solVRP.listeTournee.push_back(tournee);
        solVRP.nb_tournee++;

        i = j;
    }

    std::reverse(solVRP.listeTournee.begin(), solVRP.listeTournee.end());


    float total = 0;
    for (auto& T : solVRP.listeTournee) {
        for (int k = 0; k < T.size() - 1; k++) {
            total += pb.dist[T[k]][T[k + 1]];
        }
    }

    solVRP.cout_total_sol = total;

    return solVRP;
}












//O(n+p) + o(n)