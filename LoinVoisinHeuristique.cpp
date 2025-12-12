#include "LoinVoisinHeuristique.h"
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <vector>

T_Sol plus_loin_voisin_random(const I_Pb& pb, double p) {

    srand(time(NULL));

    int n = pb.dist.size();

    T_Sol sol;
    sol.clientsVisites.clear();
    sol.listeTournee.clear();
    sol.nb_tournee = 1;
    sol.cout_total_sol = 0;

    std::vector<int>& TG = sol.clientsVisites;
    TG.push_back(0);

    std::vector<int> L;
    for (int i = 1;i < n;i++) L.push_back(i);

    while (!L.empty()) {

        int courant = TG.back();


        int E[3] = { -1,-1,-1 };
        int dE[3] = { -1,-1,-1 };
        int nb = 0;

        for (int k : L) {
            int d = pb.dist[courant][k];

            if (nb < 3) {
                E[nb] = k;
                dE[nb] = d;
                nb++;


                for (int i = 0;i < nb;i++)
                    for (int j = i + 1;j < nb;j++)
                        if (dE[j] > dE[i]) {
                            std::swap(dE[j], dE[i]);
                            std::swap(E[j], E[i]);
                        }
            }
            else {
                int idxMin = 0;
                for (int i = 1;i < 3;i++)
                    if (dE[i] < dE[idxMin])
                        idxMin = i;

                if (d > dE[idxMin]) {
                    dE[idxMin] = d;
                    E[idxMin] = k;

                    for (int i = 0;i < 3;i++)
                        for (int j = i + 1;j < 3;j++)
                            if (dE[j] > dE[i]) {
                                std::swap(dE[j], dE[i]);
                                std::swap(E[j], E[i]);
                            }
                }
            }
        }

        
        double u = (double)rand() / RAND_MAX;
        int j;

        if (u < p) j = E[0];                         
        else if (u < p + (1 - p) * p) j = E[1];          
        else j = E[2];                              

        TG.push_back(j);
        sol.cout_total_sol += pb.dist[courant][j];

        L.erase(std::remove(L.begin(), L.end(), j), L.end());
    }

    sol.cout_total_sol += pb.dist[TG.back()][0];
    TG.push_back(0);

    sol.listeTournee.push_back(TG);

    return sol;
}
