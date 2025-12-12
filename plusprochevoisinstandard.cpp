#include "plusprochevoisinstandard.h"
#include <limits>
#include <algorithm>
#include <vector>

T_Sol plus_proche_voisins(const I_Pb& pb) {

    int n = pb.dist.size(); 

    T_Sol sol;
    sol.clientsVisites.clear();    // TG
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
        int j = -1;
        int minimum = std::numeric_limits<int>::max();

        for (int k : L) {
            int d = pb.dist[courant][k];
            if (d < minimum) {
                minimum = d;
                j = k;
            }
        }

        TG.push_back(j);
        sol.cout_total_sol += minimum;




        for (int k = 0; k < L.size(); k++) {
            if (L[k] == j) {
                L.erase(L.begin() + k);
                break;
            }
        }

    }

    int dernier = TG.back();
    sol.cout_total_sol += pb.dist[dernier][0];
    TG.push_back(0);

    sol.listeTournee.push_back(TG);

    return sol;
}




