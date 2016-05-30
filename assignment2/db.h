#ifndef DB_H
#define DB_H

typedef struct {
  int id;
  int games;
  int goals;
  int gamesFinal;
  int goalsFinal;
} player_t;

typedef struct {
  char name[40];
  int id;
  int participations;
  int games;
  int won;
  int draw;
  int lost;
  int goalsScored;
  int goalsGot;
  int difference;
  int points;
  int goals;
} team_t;

typedef struct {
  int year;
  int goals1;
  int goals2;
  int id1;
  int id2;
  int id;
  int finalRound;
} match_t;

int initDB(char* filename);
void closeDB();
void getTeam(const char* name, team_t* team);
void getMatches(team_t* team1, team_t* team2,
                match_t** matches, int* numMatches);
void getPlayersOfMatch(const match_t* match,
                       player_t** players1, int* numPlayers1,
                       player_t** players2, int* numPlayers2);

#endif // db_H
