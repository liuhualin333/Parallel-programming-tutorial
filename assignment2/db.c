#include "db.h"
#include "sqlite3.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "pthread.h"

#define MAXNUMTHREADS 32
char* FP;

int openConnection(sqlite3** db) {
  int rc = sqlite3_open_v2(
        FP, db, SQLITE_OPEN_READONLY | SQLITE_OPEN_NOMUTEX, NULL);
  if (rc) {
    sqlite3_close(*db);
    return 1;
  }
  return 0;
}

int getConnection(sqlite3** conn) {
  static sqlite3* db[MAXNUMTHREADS];
  static pthread_t con[MAXNUMTHREADS] = {0};
  int i;

  pthread_t id = pthread_self();
  for (i = 0; i < MAXNUMTHREADS; ++i) {
    if (con[i] == id) {
      *conn = db[i];
      return 0;
    } else if(con[i] == 0) {
      break;
    }
  }
  if (openConnection(db + i) != 0) {
    return 1;
  }
  con[i] = id;
  *conn = db[i];
  return 0;
}

int initDB(char* filename) {
  FP = filename;
  sqlite3* conn;
  return getConnection(&conn);
}

void closeDB() {
  sqlite3* conn;
  getConnection(&conn);
  sqlite3_close(conn);
}

void getTeam(const char* name, team_t* team) {
  sqlite3* conn;
  sqlite3_stmt *stmt;
  char buf[256];
  int stat;

  snprintf(buf, sizeof(buf), "SELECT * FROM laender WHERE Land=\"%s\"", name);
  getConnection(&conn);
  stat = sqlite3_prepare_v2(conn, buf , -1, &stmt, NULL );
  if (stat == SQLITE_OK) {
    stat = sqlite3_step(stmt);
    if (stat == SQLITE_ROW) {
      strcpy(team->name, name);
      team->id = sqlite3_column_int(stmt, 3);
      team->participations = sqlite3_column_int(stmt, 13);
      team->games = sqlite3_column_int(stmt, 15);
      team->won = sqlite3_column_int(stmt, 16);
      team->draw = sqlite3_column_int(stmt, 17);
      team->lost = sqlite3_column_int(stmt, 18);
      team->goalsScored = sqlite3_column_int(stmt, 19);
      team->goalsGot = sqlite3_column_int(stmt, 20);
      team->difference = sqlite3_column_int(stmt, 26);
      team->goals = 0;
      team->points = 0;
    }
  }

  sqlite3_finalize(stmt);
}

void fillPlayer(player_t* player) {
  sqlite3_stmt *stmt;
  sqlite3* conn;
  char buf[256];
  int stat;

  snprintf(buf, sizeof(buf), "SELECT * FROM spieler WHERE nr=\"%d\"",
           player->id);
  getConnection(&conn);
  stat = sqlite3_prepare_v2(conn, buf , -1, &stmt, NULL );
  if (stat == SQLITE_OK) {
    stat = sqlite3_step(stmt);
    if (stat == SQLITE_ROW) {
      player->games = sqlite3_column_int(stmt, 3);
      player->goals = sqlite3_column_int(stmt, 4);
      player->gamesFinal = sqlite3_column_int(stmt, 7);
      player->goalsFinal = sqlite3_column_int(stmt, 8);
    }
  }

  sqlite3_finalize(stmt);
}

int getNumMatches(const char* team1, const char* team2) {
  sqlite3* conn;
  sqlite3_stmt *stmt;
  char buf[256];
  int numMatches = 0;
  int stat;

  getConnection(&conn);

  // home first
  snprintf(buf, sizeof(buf),
      "SELECT COUNT() FROM Paarungen WHERE Heim=\"%s\" AND Gast=\"%s\"",
      team1, team2);
  stat = sqlite3_prepare_v2(conn, buf , -1, &stmt, NULL );
  if (stat == SQLITE_OK) {
    stat = sqlite3_step(stmt);
    if (stat == SQLITE_ROW){
      numMatches += sqlite3_column_int(stmt, 0);
    }
  }

  // home second
  snprintf(buf, sizeof(buf),
      "SELECT COUNT() FROM Paarungen WHERE Heim=\"%s\" AND Gast=\"%s\"",
      team2, team1);
  stat = sqlite3_prepare_v2(conn, buf , -1, &stmt, NULL );
  if (stat == SQLITE_OK) {
    stat = sqlite3_step(stmt);
    if (stat == SQLITE_ROW){
      numMatches += sqlite3_column_int(stmt, 0);
    }
  }

  sqlite3_finalize(stmt);
  return numMatches;
}

int getNumPlayersOfMatch(const match_t* match, int team) {
  sqlite3* conn;
  sqlite3_stmt *stmt;
  char buf[256];
  int numPlayers = 0;
  int stat;
  int teamId;

  if (team == 1)
    teamId = match->id1;
  else
    teamId = match->id2;


  snprintf(buf, sizeof(buf),
      "SELECT COUNT() FROM aufstellung WHERE SpielNr=\"%d\" AND LandNr=\"%d\"",
      match->id, teamId);
  getConnection(&conn);
  stat = sqlite3_prepare_v2(conn, buf , -1, &stmt, NULL );
  if (stat == SQLITE_OK) {
    stat = sqlite3_step(stmt);
    if (stat == SQLITE_ROW){
      numPlayers = sqlite3_column_int(stmt, 0);
    }
  }

  sqlite3_finalize(stmt);
  return numPlayers;
}

void getPlayersOfMatchForTeam(const match_t* match,
                              int teamNo,
                              player_t** players,
                              int* numPlayers) {
  sqlite3* conn;
  sqlite3_stmt *stmt;
  char buf[256];
  int pos = 0;
  int teamId;

  if (teamNo == 1)
    teamId = match->id1;
  else
    teamId = match->id2;

  *numPlayers = getNumPlayersOfMatch(match, teamNo);
  *players = (player_t*) calloc(*numPlayers, sizeof(player_t));

  snprintf(buf, sizeof(buf),
           "SELECT * FROM aufstellung WHERE SpielNr=\"%d\" AND LandNr=\"%d\"",
           match->id, teamId);
  getConnection(&conn);
  sqlite3_prepare_v2(conn, buf , -1, &stmt, NULL );
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    (*players)[pos].id = sqlite3_column_int(stmt, 2);
    fillPlayer(*players + pos);
    ++pos;
  }

  sqlite3_finalize(stmt);
}

void getMatchesInternal(team_t* team1, team_t* team2, int reverse,
                        match_t** matches, int* pos) {
  char buf[256];
  sqlite3* conn;
  sqlite3_stmt *stmt;
  getConnection(&conn);

  snprintf(buf, sizeof(buf),
           "SELECT * FROM Paarungen WHERE Heim=\"%s\" AND Gast=\"%s\"",
           team1->name, team2->name);
  sqlite3_prepare_v2(conn, buf , -1, &stmt, NULL );
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    (*matches)[*pos].year = sqlite3_column_int(stmt, 0);
    if (!reverse) {
      (*matches)[*pos].goals1 = sqlite3_column_int(stmt, 3);
      (*matches)[*pos].goals2 = sqlite3_column_int(stmt, 4);
      (*matches)[*pos].id1 = sqlite3_column_int(stmt, 11);
      (*matches)[*pos].id2 = sqlite3_column_int(stmt, 12);
    } else {
      (*matches)[*pos].goals2 = sqlite3_column_int(stmt, 3);
      (*matches)[*pos].goals1 = sqlite3_column_int(stmt, 4);
      (*matches)[*pos].id2 = sqlite3_column_int(stmt, 11);
      (*matches)[*pos].id1 = sqlite3_column_int(stmt, 12);
    }
    (*matches)[*pos].id = sqlite3_column_int(stmt, 13);
    (*matches)[*pos].finalRound = sqlite3_column_int(stmt, 16);
    (*pos)++;
  }
  sqlite3_finalize(stmt);
}

void getMatches(team_t* team1, team_t* team2,
                match_t** matches, int* numMatches) {
  int pos = 0;

  *numMatches = getNumMatches(team1->name, team2->name);
  *matches = malloc(sizeof(match_t) * *numMatches);

  getMatchesInternal(team1, team2, 0, matches, &pos);
  getMatchesInternal(team2, team1, 1, matches, &pos);
}

void getPlayersOfMatch(const match_t* match,
                       player_t** players1, int* numPlayers1,
                       player_t** players2, int* numPlayers2) {
  getPlayersOfMatchForTeam(match, 1, players1, numPlayers1);
  getPlayersOfMatchForTeam(match, 2, players2, numPlayers2);
}
