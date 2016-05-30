#include "emsim.h"
#include "vis.h"
#include "time.h"

int main(int argc, char *argv[])
{
  //declarations
  char *filename = "em.db";
  long int num_threads = 4;
  results = calloc(sizeof(match_result), 51);
  struct timespec begin, end;

  // argument handling
  if (argc < 2 || argc > 3) {
    fprintf(stderr, "usage: %s filename [#threads]\n", argv[0]);
    return 1;
  }
  if (argc >= 2) {
      filename = argv[1];
  }
  if (argc == 3) {
    if ((num_threads = strtol(argv[2], NULL, 0)) == 0 || num_threads < 0) {
          fprintf(stderr, "#threads not valid!\n");
          return 1;
      }
  }

  printf("Process %s \n", filename);

  team_t* teams = (team_t*) malloc(sizeof(team_t) * NUMTEAMS);

  if (initDB(filename)) {
    printf("Error during loading database: %s\n", filename);
    return 1;
  }

  // group A
  getTeam("Frankreich", teams + 0);
  getTeam("Rumänien", teams + 1);
  getTeam("Albanien", teams + 2);
  getTeam("Schweiz", teams + 3);

  // group B
  getTeam("England", teams + 4);
  getTeam("Rußland", teams + 5);
  getTeam("Wales", teams + 6);
  getTeam("Schweiz", teams + 7);

  // group C
  getTeam("Deutschland", teams + 8);
  getTeam("Ukraine", teams + 9);
  getTeam("Polen", teams + 10);
  getTeam("Nordirland", teams + 11);

  // group D
  getTeam("Spanien", teams + 12);
  getTeam("Tschechoslowakei", teams + 13);
  getTeam("Türkei", teams + 14);
  getTeam("Kroatien", teams + 15);

  // group E
  getTeam("Belgien", teams + 16);
  getTeam("Italien", teams + 17);
  getTeam("Irland", teams + 18);
  getTeam("Schweden", teams + 19);

  // group F
  getTeam("Portugal", teams + 20);
  getTeam("Island", teams + 21);
  getTeam("Österreich", teams + 22);
  getTeam("Ungarn", teams + 23);

  clock_gettime(CLOCK_MONOTONIC, &begin);
  playEM(teams, num_threads);
  clock_gettime(CLOCK_MONOTONIC, &end);

  // visualize the results
  visualizeEM();
  // print timing information
  printf("\n\nTime: %.5f seconds\n", ((double)end.tv_sec + 1.0e-9*end.tv_nsec) -
                     ((double)begin.tv_sec + 1.0e-9*begin.tv_nsec));

  closeDB();
  free(results);
  free(teams); 
 
  return 0;
}
