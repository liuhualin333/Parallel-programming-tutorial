#include "emsim.h"
#include <omp.h>

void playGroups(team_t* teams)
{
 // put your code here
  static const int cNumTeamsPerGroup = NUMTEAMS / NUMGROUPS;
  int g, i;
  int goalsI, goalsJ;
  #pragma omp parallel private(g,i)
  {
  for (g = 0; g < NUMGROUPS; ++g) {
    for (i =  g * cNumTeamsPerGroup; i < (g+1) * cNumTeamsPerGroup; ++i) {
      #pragma omp for nowait schedule(dynamic) private (goalsI, goalsJ)
      for (int j = (g+1) * cNumTeamsPerGroup - 1; j > i; --j) {
        // team i plays against team j in group g
        playGroupMatch(g, teams + i, teams + j, &goalsI, &goalsJ);
        teams[i].goals += goalsI - goalsJ;
        teams[j].goals += goalsJ - goalsI;
        if (goalsI > goalsJ)
          teams[i].points += 3;
        else if (goalsI < goalsJ)
          teams[j].points += 3;
        else {
          teams[i].points += 1;
          teams[j].points += 1;
        }
      }
  }
    }
  }
}

void playFinalRound(int numGames, team_t** teams, team_t** successors)
{
 // put your code here
  team_t* buffer[16];
  team_t* team1;
  team_t* team2;
  
  int goals1 = 0, goals2 = 0;
#pragma omp parallel 
{
  #pragma omp for schedule(dynamic) private(team1, team2, goals1, goals2)
  for (int i = 0; i < numGames; ++i) {
    goals1 = 0; goals2 = 0;
    team1 = teams[i*2];
    team2 = teams[i*2+1];
    playFinalMatch(numGames, i, team1, team2, &goals1, &goals2);
    if (goals1 > goals2)
      buffer[i] = team1;
    else if (goals1 < goals2)
      buffer[i] = team2;
    else {
      playPenalty(team1, team2, &goals1, &goals2);
      if (goals1 > goals2)
        buffer[i] = team1;
      else
        buffer[i] = team2;
    }
  }
  #pragma omp for schedule(dynamic)
  for (int i = 0; i < numGames; ++i){
    successors[i] = buffer[i];
  }
}
} 
