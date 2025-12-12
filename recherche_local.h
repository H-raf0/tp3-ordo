#pragma once
#include "instance.h"

// 2-OPT sur chaque tournée
void deuxOpt(T_Sol& solVRP, const I_Pb& pb);

// déplacement d'un client entre tournées
void deplacerClient(T_Sol& solVRP, const I_Pb& pb);

// coût total d'une solution VRP
float cout_solution(const T_Sol& sol, const I_Pb& pb);
