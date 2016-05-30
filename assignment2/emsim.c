#include "emsim.h"
#include "vis.h"

static int gameNum[NUMGROUPS];

void getGroupWinners(team_t* teams, int numTeams,
                     team_t** first, team_t** second, team_t** third) {
  int i, j, dominates;

  // initialize winners to consider wrong user functions
  *first = teams + 0;
  *second = teams + 1;
  *third = teams + 2;

  // return first, second, and third team
  for (i = 0; i < numTeams; ++i) {
    dominates = 0;
    for (j = numTeams - 1; j >= 0; --j) {
      if (i != j && team1DominatesTeam2(teams + i, teams + j)) {
        dominates += 1;
      }
    }
    if (dominates == numTeams - 1) *first = teams + i;
    else if (dominates == numTeams - 2) *second = teams + i;
    else if (dominates == numTeams - 3) *third = teams + i;
  }
}

void playEM(team_t* teams, int numThreads) {
  static const int cInitialNumSuccessors = NUMGROUPS * 2 + NUMTHIRDS;
  static const int cTeamsPerGroup = NUMTEAMS / NUMGROUPS;
  team_t* successors[NUMGROUPS * 2 + NUMTHIRDS] = {0};
  team_t* bestThirds[NUMGROUPS];
  int g, curBestThird = 0, numSuccessors = cInitialNumSuccessors;

  initialize();
  playGroups(teams, numThreads-1);

  for (g = 0; g < NUMGROUPS; ++g) {
    getGroupWinners(teams + (g * cTeamsPerGroup),
              cTeamsPerGroup,
              successors + g * 2,
              successors + (numSuccessors - (g * 2) - 1),
              bestThirds + g);
  }

  // fill best thirds
  sortTeams(NUMGROUPS, bestThirds);
  for (g = 0; g < numSuccessors; ++g) {
    if (successors[g] == NULL) {
      successors[g] = bestThirds[curBestThird++];
    }
  }

  // play final rounds
  while (numSuccessors > 1) {
    playFinalRound(numSuccessors / 2, successors, successors, numThreads -1);
    numSuccessors /= 2;
  }
}

void initialize() {
  memset(&gameNum, 0, NUMGROUPS * sizeof(int));
}

int team1DominatesTeam2(team_t* team1, team_t* team2)
{
  if (team1->points > team2->points ||
      (team1->points == team2->points && team1->goals > team2->goals))
    return true;
  else
    return false;
}

double getGoalsPerGame(const player_t* players, int numPlayers) {
  int i;
  double goalsPerGame = 0.0;

  for (i = 0; i < numPlayers; ++i) {
    if (players[i].games) {
      goalsPerGame += (double)players[i].goals / (double)players[i].games;
    }
  }

  return goalsPerGame;
}

void playMatchGen(team_t* team1, team_t* team2, int* goals1, int* goals2) {
  match_t* matches;
  player_t *players1, *players2;
  int i, numMatches, numPlayers1, numPlayers2;
  double g1 = 0.0, g2 = 0.0;

  getMatches(team1, team2, &matches, &numMatches);
  if (numMatches) {
    for (i=0; i < numMatches; ++i) {
      getPlayersOfMatch(matches + i,
                        &players1, &numPlayers1,
                        &players2, &numPlayers2);
      g1 += getGoalsPerGame(players1, numPlayers1);
      free(players1);
      g2 += getGoalsPerGame(players2, numPlayers2);
      free(players2);
      g1 += (double)matches[i].goals1 / (double)numMatches;
      g2 += (double)matches[i].goals2 / (double)numMatches;
    }
    free(matches);
  } else if (team1->difference > team2->difference) {
    g1 = 1;
    g2 = 0;
  } else {
    g2 = 1;
    g1 = 0;
  }
  *goals1 = g1;
  *goals2 = g2;
}

void playGroupMatch(int groupNo,
                    team_t* team1, team_t* team2, int* goals1, int* goals2) {
  playMatchGen(team1, team2, goals1, goals2);

  // handle visualization
  handleGame(gameNum[groupNo] + NUMGROUPS * groupNo,
             team1->name, team2->name, *goals1, *goals2);
  gameNum[groupNo]++;
}

int final_pos (int numGames){
  if (numGames == 8)
    return 0;
  return  final_pos (2 * numGames)+ 2*numGames;
}

void playFinalMatch(int numGames, int gameNo,
                    team_t* team1, team_t* team2, int* goals1, int* goals2) {
  playMatchGen(team1, team2, goals1, goals2);

  // handle visualization
  handleGame(36 + final_pos(numGames) + gameNo,
             team1->name, team2->name, *goals1, *goals2);
}

void playPenalty(team_t* team1, team_t* team2,
                 int* goals1, int* goals2) {
  match_t* matches;
  int numMatches;
  int i;
  int diff = 0;

  getMatches(team1, team2, &matches, &numMatches);
  for (i = 0; i < numMatches; ++i) {
    if (matches[i].finalRound) {
      diff += matches[i].goals1;
      diff -= matches[i].goals2;
    }
    if (diff >= 0) {
      *goals1 += 1;
      *goals2 += 0;
    }
    else {
      *goals1 += 0;
      *goals2 += 1;
    }
  }
  free(matches);
}

void swapTeams(team_t** team1, team_t** team2) {
  team_t* tmp;
  tmp = *team1;
  *team1 = *team2;
  *team2 = tmp;
}

void sortTeams(int numTeams, team_t** teams) {
  int i, j;
  for (i = 0; i < numTeams; ++i) {
    for (j = numTeams - 1; j > i; --j) {
      if (team1DominatesTeam2(teams[j], teams[i])) {
        swapTeams(teams + i, teams + j);
      }
    }
  }
}
