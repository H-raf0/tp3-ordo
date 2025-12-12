#pragma once
#include <string>
#include <vector>


struct I_Pb {
	std::vector<int> qte;
	std::vector<std::vector<int>> dist;
	int capacite;
	int nb_vehicules;
};


struct T_tournee {
	std::vector<int> villes;
	int cout;
	int qte;
};




struct T_Sol {
	std::vector<int> clientsVisites;
	int nb_tournee;
	float cout_total_sol;
	std::vector<std::vector<int>> listeTournee;
};






void lire_instance(const std::string& nom_fichier, I_Pb& pb);

void generer_vecteur(const I_Pb& pb, T_Sol& s);



