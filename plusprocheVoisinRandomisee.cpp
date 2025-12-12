#include "plusprocheVoisinRandomisee.h"
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <limits>

T_Sol plus_proche_voisins_random(const I_Pb& pb) {

    int n = pb.dist.size();


    srand(time(NULL));

    T_Sol sol;


    sol.clientsVisites.clear();
    sol.listeTournee.clear();
    sol.nb_tournee = 1;
    sol.cout_total_sol = 0;

    std::vector<int>& TG = sol.clientsVisites;
    TG.push_back(0);

    std::vector<int> L;
    for (int i = 1; i < n; i++)
        L.push_back(i);

    while (!L.empty()) {

        int courant = TG.back();

        // Construction de E (3 plus proches voisins)
      
        int E[3] = { -1, -1, -1 };
        int dE[3] = { 999999999, 999999999, 999999999 };
        int c = 0;

        for (int k : L) {

            int d = pb.dist[courant][k];

            if (c < 3) {
                E[c] = k;
                dE[c] = d;
                c++;

                for (int i = 0; i < c - 1; i++)
                    for (int j = i + 1; j < c; j++)
                        if (dE[j] < dE[i]) {
                            std::swap(dE[i], dE[j]);
                            std::swap(E[i], E[j]);
                        }
            }
            else {
                int idxMax = 0;
                for (int i = 1; i < 3; i++)
                    if (dE[i] > dE[idxMax])
                        idxMax = i;

                if (d < dE[idxMax]) {
                    dE[idxMax] = d;
                    E[idxMax] = k;

                    for (int i = 0; i < 2; i++)
                        for (int j = i + 1; j < 3; j++)
                            if (dE[j] < dE[i]) {
                                std::swap(dE[i], dE[j]);
                                std::swap(E[i], E[j]);
                            }
                }
            }
        }



        double p = 0.8;   

        double u = (double)rand() / RAND_MAX;

        int j;

        if (u < p) {
            j = E[0];  
        }
        else if (u < p + (1 - p) * p) {
            j = E[1];  
        }
        else {
            j = E[2];  
        }

      

        TG.push_back(j);
        sol.cout_total_sol += pb.dist[courant][j];

        for (int k = 0; k < L.size(); k++) {
            if (L[k] == j) {
                L.erase(L.begin() + k);
                break;
            }
        }
    }

    sol.cout_total_sol += pb.dist[TG.back()][0];
    TG.push_back(0);

    sol.listeTournee.push_back(TG);

    return sol;
}
