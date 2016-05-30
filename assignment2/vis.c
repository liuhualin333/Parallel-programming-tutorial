#include "vis.h"
#include "emsim.h"

void visualize(int pos, int numMatches){

  match_result* r = results + pos;

    // displays the results given as parameters in a single row
 	// must be called for every group and final round of same level

	char goals1[numMatches][33];
	char goals2[numMatches][33];
	int i;

	for(i = 0; i < numMatches; i++){
		if(numMatches == 4) printf("            ");
		if(numMatches == 2) printf("                                    ");
		if(numMatches == 1) printf("                                                                                    ");
    sprintf(goals1[i], "%d", r[i].goals1);
    sprintf(goals2[i], "%d", r[i].goals2);
		printf("----------------------- ");
		if(numMatches == 4) printf("            ");
		if(numMatches == 2) printf("                                    ");
		
	}
	printf("\n");

	for(i = 0; i < numMatches; i++){
		if(numMatches == 4) printf("            ");
		if(numMatches == 2) printf("                                    ");
		if(numMatches == 1) printf("                                                                                    ");

		//some issues with German characters :)
    if(!strcmp(r[i].name1,"Rußland") || !strcmp(r[i].name1,"Rumänien") || !strcmp(r[i].name1,"Türkei") || !strcmp(r[i].name1,"Österreich"))
      printf("|%*s%*s-", 8+(int)strlen(r[i].name1)/2, r[i].name1, 9-(int)strlen(r[i].name1)/2, "");
		else 
      printf("|%*s%*s-", 8+(int)strlen(r[i].name1)/2, r[i].name1, 8-(int)strlen(r[i].name1)/2, "");

		printf("%*s%*s| ", 2+(int)strlen(goals1[i])/2,  goals1[i], 2-(int)strlen(goals1[i])/2, "");

		if(numMatches == 4) printf("            ");
		if(numMatches == 2) printf("                                    ");
		
	}
	printf("\n");
	
	for(i = 0; i < numMatches; i++)	{

		if(numMatches == 4) printf("            ");
		if(numMatches == 2) printf("                                    ");
		if(numMatches == 1) printf("                                                                                    ");
		
    if(!strcmp(r[i].name2,"Rußland") || !strcmp(r[i].name2,"Rumänien") || !strcmp(r[i].name2,"Türkei") || !strcmp(r[i].name2,"Österreich"))
      printf("|%*s%*s-", 8+(int)strlen(r[i].name2)/2, r[i].name2, 9-(int)strlen(r[i].name2)/2, "" );
		else 
      printf("|%*s%*s-", 8+(int)strlen(r[i].name2)/2, r[i].name2, 8-(int)strlen(r[i].name2)/2, "" );

		printf("%*s%*s| ", 2+(int)strlen(goals2[i])/2, goals2[i], 2-(int)strlen(goals2[i])/2, "");

		if(numMatches == 4) printf("            ");
		if(numMatches == 2) printf("                                    ");
		
	}
	printf("\n");

	for(i = 0; i < numMatches; i++){

		if(numMatches == 4) printf("            ");
		if(numMatches == 2) printf("                                    ");
		if(numMatches == 1) printf("                                                                                    ");

		printf("----------------------- ");
		if(numMatches == 4) printf("            ");
		if(numMatches == 2) printf("                                    ");
		
	}
	printf("\n\n");
	
}

void visualizeEM() {
  int g;
  int numGames = (NUMGROUPS * 2 + NUMTHIRDS)/2;
  for (g = 0; g < NUMGROUPS; ++g)
  {
    printf("     Group %c\n", g + 65);
    visualize(6 * g, 6);
  }
  printf("	Finals!\n");
  while (numGames >= 1) {
    visualize(36 + final_pos(numGames), numGames);
    numGames /= 2;
  }
}

void handleGame(int index,
                const char* team1, const char* team2,
                int goals1, int goals2) {
  strcpy(results[index].name1 , team1);
  strcpy(results[index].name2 , team2);
  results[index].goals1 = goals1;
  results[index].goals2 = goals2;
}
