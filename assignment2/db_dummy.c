#include "db.h"
#include "sqlite3.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "pthread.h"

char* FP;

int openConnection(sqlite3** db) {
  return 0;
}

int getConnection(sqlite3** conn) {
  return 0;
}

int initDB(char* filename) {
  return 0;
}

void closeDB() {
}

void getTeam(const char* name, team_t* team) {
  strcpy(team->name,"Dummy");
  team->id = 0;
  team->participations = 1;
  team->games = 1;
  team->won = 1;
  team->draw = 1;
  team->lost = 1;
  team->goalsScored = 1;
  team->goalsGot = 1;
  team->difference = 1;
  team->goals = 1;
  team->points = 1;
}

void fillPlayer(player_t* player) {
  player->games = 1;
  player->goals = 1;
  player->gamesFinal = 1;
  player->goalsFinal = 1;
}

int getNumMatches(const char* team1, const char* team2) {
  return 0;
}

int getNumPlayersOfMatch(const match_t* match, int team) {
  return 0;
}

void getPlayersOfMatchForTeam(const match_t* match,
                              int teamNo,
                              player_t** players,
                              int* numPlayers) {
  *numPlayers = 0;
}

void getMatchesInternal(team_t* team1, team_t* team2, int reverse,
                        match_t** matches, int* pos) {
}

void getMatches(team_t* team1, team_t* team2,
                match_t** matches, int* numMatches) {
 *numMatches = 0;
}

void getPlayersOfMatch(const match_t* match,
                       player_t** players1, int* numPlayers1,
                       player_t** players2, int* numPlayers2) {
  *numPlayers1 = 0;
  *numPlayers2 = 0;
}
