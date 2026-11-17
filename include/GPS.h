#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

//-----------------------------------------------------------------------------
//Définition d'un type « string » comme un tableau de 50 caractères 
//-----------------------------------------------------------------------------

typedef char string[50];

//-----------------------------------------------------------------------------
//Constantes utilisées dans le programme
//-----------------------------------------------------------------------------

#define MAX_FACTS 100
#define MAX_RULES 100
#define MAX_FACT_LENGTH 50
#define MAX_ACTION_LENGTH 100
#define MAX_PRECONDS 10
#define MAX_ADD 10
#define MAX_DELETE 10
#define MAX_LINE_LENGTH 256
#define MAX_DEPTH 1000

//-----------------------------------------------------------------------------
//Codes ANSI pour la couleur (pour l'affichage)
//-----------------------------------------------------------------------------
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define BOLD    "\033[1m"

//-----------------------------------------------------------------------------
//Structures de données pour représenter un état (ensemble de faits) et 
//une règle (action, préconditions, faits à ajouter et faits à supprimer)
//-----------------------------------------------------------------------------
typedef struct{
    char facts[MAX_FACTS][MAX_FACT_LENGTH];
    int factCount;
} State;

typedef struct{
    char action[MAX_ACTION_LENGTH];
    char preconds[MAX_PRECONDS][MAX_FACT_LENGTH];
    int precondCount;
    char add[MAX_ADD][MAX_FACT_LENGTH];
    int addCount;
    char del[MAX_DELETE][MAX_FACT_LENGTH];
    int delCount;
} Rule;

//-----------------------------------------------------------------------------
//Variables globales pour stocker le problème, les règles et la solution trouvée
//-----------------------------------------------------------------------------
extern State startState;
extern State finishState;
extern Rule rules[MAX_RULES];
extern int ruleCount;

extern bool solutionFound; //Indique qu'une solution a été trouvée
extern char solutionActions[MAX_DEPTH][MAX_ACTION_LENGTH];
extern State solutionPath[MAX_DEPTH+1]; //solutionPath[i] correspond à l'état après i actions
extern int solutionLength; //Nombre d'actions dans la solution


//------------------------
//Prototypes des fonctions
//------------------------
void sep();
void printState(State* s);
void printSolution();
void trim(char* s);
void parseError(int lineNo, const char* msg);
bool verifLine(FILE *file, char *buffer, int *lineNo);
int parseLine(const char *line, const char* key, string cible[], int maxTokens, int lineNo);
bool parseState(const char *line, const char *key, State *s, int lineNo);
bool parseAction(FILE *file, char *line, int *lineNo);
bool loadProblem(const char *path);
void solve(State current, State path[], int depth, int startRule);
bool stateInPath(State path[], int depth, State* s);
bool containsFact(State* s, const char* fact);
void addFact(State* s, const char* fact);
void removeFact(State* s, const char* fact);
bool goalReached(State* s, State* goal);
bool ruleApplicable(State* s, Rule* r);
void applyRule(State* s, Rule* r, State* newState);
bool stateEquals(State* s1, State* s2);