#include "grasp.h"
#include "plusprocheVoisinRandomisee.h"  
#include "evaluer.h"                      
#include "recherche_local.h"              
#include <cstdlib>
#include <ctime>

T_Sol grasp_vrp(const I_Pb& pb, int maxIter) {

    std::srand((unsigned)std::time(nullptr));

    T_Sol best;
    best.cout_total_sol = 1e9f;

    int total = (int)pb.dist.size();

    for (int it = 0; it < maxIter; ++it) {

        // 1) construction TSP aléatoire
        T_Sol solTSP = plus_proche_voisins_random(pb);

       
        // sécurité indices (0 <= v < total)
        bool ok = true;
        for (int v : solTSP.clientsVisites) {
            if (v < 0 || v >= total) {
                ok = false;
                break;
            }
        }
        if (!ok) continue;

        // 2) split VRP (flotte illimitée ici)
        T_Sol solVRP = split(pb, solTSP);

        if (solVRP.listeTournee.empty())
            continue;

        // 3) recherche locale
        deuxOpt(solVRP, pb);

        // 4) mise à jour du best
        if (solVRP.cout_total_sol < best.cout_total_sol) {
            best = solVRP;
        }
    }

    return best;
}
