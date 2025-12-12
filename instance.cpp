#include <fstream>
#include <iostream>
#include <algorithm>
#include "instance.h"

void lire_instance(const std::string& nom_fichier, I_Pb& pb) {

    std::ifstream f(nom_fichier);
    if (!f) {
        std::cerr << "Erreur : impossible d'ouvrir " << nom_fichier << std::endl;
        return;
    }

    int n, depot;
    f >> n;                     
    int total = n + 1;          

    f >> pb.nb_vehicules >> pb.capacite;  
    f >> depot;                 

    pb.dist.assign(total, std::vector<int>(total));

    for (int i = 0; i < total; i++)
        for (int j = 0; j < total; j++)
            f >> pb.dist[i][j];

    std::string trash;
    std::getline(f, trash);
    std::getline(f, trash);

    pb.qte.assign(total, 0);
    int idx, q;
    while (f >> idx >> q)
        if (idx >= 1 && idx <= n)
            pb.qte[idx] = q;
}





void generer_vecteur(const I_Pb& pb, T_Sol& s) {
    int total = pb.dist.size();
    s.clientsVisites.clear();
    s.clientsVisites.push_back(0);       
    for (int i = 1; i < total; i++)
        s.clientsVisites.push_back(i);
}
