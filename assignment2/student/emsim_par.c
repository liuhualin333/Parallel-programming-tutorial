#include "emsim.h"
#include <pthread.h>
typedef struct {
  pthread_mutex_t mutex;
  int value;
} mutex_str;

static mutex_str mux = {PTHREAD_MUTEX_INITIALIZER, 0};

typedef struct data {
  team_t** teams;
  int numGames;
  team_t* teami;
  team_t* teamj;
  team_t* team1;
  team_t* team2;
  int groupnum;
  char dummy[64];
} data;

typedef struct stack {
  data dataarr[36];
  int size;
  int maxnum;
} Stack;


static Stack stack;
static team_t** teamsptr;
static int gamesNum;

void Stack_Init(Stack *S)
{
    S->size = 0;
    S->maxnum = 36;
}

data Stack_Top(Stack *S)
{
    if (S->size == 0) {
    } 

    return S->dataarr[S->size-1];
}

int Stack_Push(Stack *S, data d)
{
    if (S->size < S->maxnum){
        S->dataarr[S->size++] = d;
        return 1;
    }
    else {
        return -1;
    }
}

int Stack_Pop(Stack *S)
{
    if (S->size == 0){
        return -1;
    }
    else {
        S->size--;
        return 1;
    }
}

void * worker_thread(void* ptr){
  int goalsI, goalsJ;
  //int num = 0;
  data elm;
  while(mux.value < 36) {
  pthread_mutex_lock(&mux.mutex);
  if(stack.size != 0){
    mux.value += 1;
    elm = Stack_Top(&stack);
    Stack_Pop(&stack);
    pthread_mutex_unlock(&mux.mutex);
    goalsI = 0;
    goalsJ = 0;
    playGroupMatch(elm.groupnum, elm.teami, elm.teamj, &goalsI, &goalsJ);
    (*(elm.teami)).goals += goalsI - goalsJ;
    (*(elm.teamj)).goals += goalsJ - goalsI;
    if (goalsI > goalsJ)
      (*(elm.teami)).points += 3;
    else if (goalsI < goalsJ)
      (*(elm.teamj)).points += 3;
    else {
      (*(elm.teami)).points += 1;
      (*(elm.teamj)).points += 1;
    }
  }
  else{
    pthread_mutex_unlock(&mux.mutex);
    continue;
  }
  }
return NULL;
}
void * worker_thread_final(void* ptr) {
  data elm;
  int goals1 = 0, goals2 = 0;
  while(mux.value < gamesNum){
    pthread_mutex_lock(&mux.mutex);
    if(stack.size != 0){
      elm = Stack_Top(&stack);
      Stack_Pop(&stack);
      mux.value += 1;
      pthread_mutex_unlock(&mux.mutex);
      playFinalMatch(gamesNum, elm.numGames, elm.team1, elm.team2, &goals1, &goals2);
      if (goals1 > goals2)
        teamsptr[elm.numGames] = elm.team1;
      else if (goals1 < goals2)
        teamsptr[elm.numGames] = elm.team2;
      else {
        playPenalty(elm.team1, elm.team2, &goals1, &goals2);
        if (goals1 > goals2)
	  teamsptr[elm.numGames] = elm.team1;
        else
          teamsptr[elm.numGames] = elm.team2;
      }
   }
   else{
      pthread_mutex_unlock(&mux.mutex);
      continue;
   }
  }
  return NULL;
}

void playGroups(team_t* teams, int numWorker)
{
 // put your code here
  static const int cNumTeamsPerGroup = NUMTEAMS / NUMGROUPS;
  int count;
  int g, i, j;
  data elm;
  //pthread_t producer;
  pthread_t *worker;
  worker = malloc(numWorker * sizeof(*worker));

  pthread_mutex_lock(&mux.mutex);
  Stack_Init(&stack);
  pthread_mutex_unlock(&mux.mutex);
/*initialization of data*/
  elm.teams = NULL;
  elm.numGames = 0;
  elm.teami = NULL;
  elm.teamj = NULL;
  elm.team1 = NULL;
  elm.team2 = NULL;
  elm.groupnum = 0;
  count = 0;
  for (g = 0; g < NUMGROUPS; ++g) {
    for (i =  g * cNumTeamsPerGroup; i < (g+1) * cNumTeamsPerGroup; ++i) {
      for (j = (g+1) * cNumTeamsPerGroup - 1; j > i; --j) {
        elm.teami = teams + i;
        elm.teamj = teams + j;
        elm.groupnum = g;
        pthread_mutex_lock(&mux.mutex);
        Stack_Push(&stack, elm);
        pthread_mutex_unlock(&mux.mutex);
        count ++;
      }
    }
  }

//  pthread_create(&producer, NULL, &producer_thread, teams);
  for(count = 0; count < numWorker; ++count){
    pthread_create(worker + count, NULL, &worker_thread, NULL);

  }
//  pthread_join(producer, NULL);
  for(count = 0; count < numWorker; ++count){
    pthread_join(worker[count], NULL);
  }

  free(worker);
}
void playFinalRound(int numGames, team_t** teams, team_t** successors, int numWorker)
{
 // put your code here
  //pthread_t producer;
  pthread_t *worker;
  int i = 0;	
  int count = 0;
  data elm;

  mux.value = 0;
  teamsptr = teams;
  gamesNum = numGames;

  pthread_mutex_lock(&mux.mutex);
  Stack_Init(&stack);
  stack.maxnum = gamesNum;
  pthread_mutex_unlock(&mux.mutex);
  /*initialization of data*/
  elm.teams = NULL;
  elm.numGames = 0;
  elm.teami = NULL;
  elm.teamj = NULL;
  elm.team1 = NULL;
  elm.team2 = NULL;
  elm.groupnum = 0;
  for (i = 0; i < gamesNum; ++i) {
    elm.team1 = teamsptr[i*2];
    elm.team2 = teamsptr[i*2+1];
    elm.numGames = i;
    pthread_mutex_lock(&mux.mutex);
    Stack_Push(&stack, elm);
    pthread_mutex_unlock(&mux.mutex);
  }
  if (numGames >= 8){
  worker = malloc((numWorker)* sizeof(*worker));
  for(count = 0; count < numWorker; count ++){
    pthread_create(worker + count, NULL, &worker_thread_final, NULL);
  }
  for(count = 0; count < numWorker; count ++){
    pthread_join(worker[count],NULL);
  }
  }
  else {
  worker = malloc((numWorker - 1)* sizeof(*worker));
  for(count = 0; count < numWorker - 1; count ++){
    pthread_create(worker + count, NULL, &worker_thread_final, NULL);
  }
  for(count = 0; count < numWorker - 1; count ++){
    pthread_join(worker[count],NULL);
  }
  }
  free(worker);
} 
