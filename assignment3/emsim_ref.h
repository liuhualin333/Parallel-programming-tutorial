#ifndef EM_REF_H
#define EM_REF_H

#include "emsim.h"

void playGroups_ref(team_t* teams);
void playFinalRound_ref(int numGames, team_t** teams, team_t** successors);
void playEM_ref(team_t* teams); 

#endif // EM_REF_H
