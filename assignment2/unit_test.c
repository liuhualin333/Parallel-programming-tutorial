#include "emsim_ref.h"

#define NUMGAMES 51
#define NUMTEAMS 24
#define NUMGROUPS 6

int main(int argc, char **argv)
{
	//declaration of the variables	
	char *filename = "em.db";
	int nThreads, i, j, failed = 1;
	match_result *results_ref;
	results_ref = malloc(51 * sizeof(match_result));
	results     = malloc(51 * sizeof(match_result));

	//argument handling 
  	if (argc != 2) {
	    fprintf(stderr, "usage: %s filename\n", argv[0]);
	    return 1;
	}
 
	if (argc >= 2)
		filename = argv[1];
	

	printf("Process %s\n", filename);
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

	//get the reference result
	printf("running reference sequential code\n");
	playEM_ref(teams);
	memcpy(results_ref, results, 51 * sizeof *results);
	memset(results, 0, 51 * sizeof *results);

	/* start the tests */
	for (nThreads = 4; nThreads <= 4; nThreads++) {
		//get the parallel result
		printf("running parallel code with %d thread(s)\n", nThreads);
	        playEM(teams, nThreads);
		for (i = 0; i < NUMGAMES; i++) {
			failed = 1;
		    	for (j = 0; j < NUMGAMES; j++) {
				if (!strcmp(results_ref[i].name1, results[j].name1)  &&
				    !strcmp(results_ref[i].name2, results[j].name2)  &&
				    results_ref[i].goals1 == results[j].goals1 &&
				    results_ref[i].goals2 == results[j].goals2 
				    ) {
					
					failed = 0;
					break;
				}

			}

			if (failed == 1){
				fprintf(stderr, "Computation with %d threads failed:\n", nThreads);
				fprintf(stderr, "Wrong result for %d: %s (correct: %s)\n",
					i, results[i].name1, results_ref[i].name1);
				fprintf(stderr, "Wrong result for %d: %s (correct: %s)\n",
					i, results[i].name2, results_ref[i].name2);
				fprintf(stderr, "Wrong result for %d: %d (correct: %d)\n",
					i, results[i].goals1, results_ref[i].goals1);
				fprintf(stderr, "Wrong result for %d: %d (correct: %d)\n",
					i, results[i].goals2, results_ref[i].goals2);
				//printf("YOUR CODE FAILED THE UNIT TEST\n");
				break;
			}

		}
		if (failed == 1){
			printf("YOUR CODE FAILED THE UNIT TEST\n");
			break;
		}
	}
	
	if (failed == 0)
		printf("YOUR CODE PASSED THE UNIT TEST\n");

	/* finish the tests */

	closeDB();
	free (results);
	free (results_ref);

	return failed;
}
