#pragma once
#include "instance.h"



T_Sol evaluer(const I_Pb& pb, const T_Sol& solTSP);
T_Sol evaluer_nb_camion(const I_Pb& pb, const T_Sol& solTSP);
T_Sol split(const I_Pb& pb, const T_Sol& solTSP);
