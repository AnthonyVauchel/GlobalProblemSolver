#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//-----------------------------------------------------------------------------
// Couleurs ANSI
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
// Types et constantes
//-----------------------------------------------------------------------------

typedef char string[50];

#define MAX_FACTS 100
#define MAX_RULES 100
#define MAX_FACT_LENGTH 50
#define MAX_ACTION_LENGTH 100
#define MAX_PRECONDS 10
#define MAX_ADD 10
#define MAX_DELETE 10
#define MAX_LINE_LENGTH 256

typedef struct {
    char facts[MAX_FACTS][MAX_FACT_LENGTH];
    int factCount;
} State;

typedef struct {
    char action[MAX_ACTION_LENGTH];
    char preconds[MAX_PRECONDS][MAX_FACT_LENGTH];
    int precondCount;
    char add[MAX_ADD][MAX_FACT_LENGTH];
    int addCount;
    char del[MAX_DELETE][MAX_FACT_LENGTH];
    int delCount;
} Rule;

//-----------------------------------------------------------------------------
// Variables globales
//-----------------------------------------------------------------------------

State currentState;
State goalState;
Rule rules[MAX_RULES];
int ruleCount = 0;

//-----------------------------------------------------------------------------
// Fonctions utilitaires
//-----------------------------------------------------------------------------

void trimNewline(char* s) {
    char* p = strchr(s, '\n');
    if (p) *p = '\0';
}

int parseLine(char source[], string cible[]) {
    int i = 0, n = 0;
    while (source[i] != ':' && source[i] != '\0') i++;
    if (source[i] == '\0') return 0;
    i++;
    while (source[i] == ' ') i++;

    int j = i;
    while (source[i] != '\n' && source[i] != '\0') {
        if (source[i] == ',') {
            int len = i - j;
            memcpy(cible[n], &source[j], len);
            cible[n][len] = '\0';
            n++;
            i++;
            while (source[i] == ' ') i++;
            j = i;
        } else {
            i++;
        }
    }
    if (i > j) {
        int len = i - j;
        memcpy(cible[n], &source[j], len);
        cible[n][len] = '\0';
        n++;
    }
    return n;
}

bool containsFact(State* s, const char* fact) {
    for (int i = 0; i < s->factCount; i++) {
        if (strcmp(s->facts[i], fact) == 0) return true;
    }
    return false;
}

void addFact(State* s, const char* fact) {
    if (!containsFact(s, fact) && s->factCount < MAX_FACTS) {
        strcpy(s->facts[s->factCount], fact);
        s->factCount++;
    }
}

void removeFact(State* s, const char* fact) {
    for (int i = 0; i < s->factCount; i++) {
        if (strcmp(s->facts[i], fact) == 0) {
            for (int j = i; j < s->factCount - 1; j++) {
                strcpy(s->facts[j], s->facts[j + 1]);
            }
            s->factCount--;
            break;
        }
    }
}

bool ruleApplicable(State* s, Rule* r) {
    for (int i = 0; i < r->precondCount; i++) {
        if (!containsFact(s, r->preconds[i])) return false;
    }
    return true;
}

void applyRule(State* s, Rule* r) {
    for (int i = 0; i < r->delCount; i++) {
        removeFact(s, r->del[i]);
    }
    for (int i = 0; i < r->addCount; i++) {
        addFact(s, r->add[i]);
    }
}

bool goalReached(State* s, State* goal) {
    for (int i = 0; i < goal->factCount; i++) {
        if (!containsFact(s, goal->facts[i])) return false;
    }
    return true;
}

void printState(State* s) {
    printf(YELLOW);
    for (int i = 0; i < s->factCount; i++) {
        printf("\"%s\"", s->facts[i]);
        if (i < s->factCount - 1) printf(", ");
    }
    printf(RESET "\n");
}

void sep() {
    printf("\n");
    for (int i = 0; i < 70; i++) printf("-");
    printf("\n\n");
}

//-----------------------------------------------------------------------------
// Main : moteur de chaînage avant
//-----------------------------------------------------------------------------

int main() {
    char filePath[256];
    printf(BOLD "Chemin du fichier de règles : " RESET);
    fgets(filePath, sizeof(filePath), stdin);
    trimNewline(filePath);

    FILE* file = fopen(filePath, "r");
    if (!file) {
        printf(RED "Erreur ouverture fichier.\n" RESET);
        return 1;
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, file)) {
        trimNewline(line);
        if (strlen(line) == 0 || strcmp(line, "****") == 0) continue;

        if (strncmp(line, "start:", 6) == 0) {
            string tokens[MAX_FACTS];
            int n = parseLine(line, tokens);
            currentState.factCount = 0;
            for (int i = 0; i < n; i++) {
                addFact(&currentState, tokens[i]);
            }
        } else if (strncmp(line, "finish:", 7) == 0) {
            string tokens[MAX_FACTS];
            int n = parseLine(line, tokens);
            goalState.factCount = 0;
            for (int i = 0; i < n; i++) {
                addFact(&goalState, tokens[i]);
            }
        } else if (strncmp(line, "action:", 7) == 0) {
            Rule r;
            string tokens[MAX_FACTS];
            int n = parseLine(line, tokens);
            if (n > 0) strcpy(r.action, tokens[0]);

            fgets(line, MAX_LINE_LENGTH, file);
            r.precondCount = parseLine(line, r.preconds);

            fgets(line, MAX_LINE_LENGTH, file);
            r.addCount = parseLine(line, r.add);

            fgets(line, MAX_LINE_LENGTH, file);
            r.delCount = parseLine(line, r.del);

            rules[ruleCount++] = r;
        }
    }
    fclose(file);

    sep();
    printf(BOLD "État initial : " RESET);
    printState(&currentState);
    printf(BOLD "Objectif : " RESET);
    printState(&goalState);
    sep();

    while (!goalReached(&currentState, &goalState)) {
        bool ruleApplied = false;
        for (int i = 0; i < ruleCount; i++) {
            if (ruleApplicable(&currentState, &rules[i])) {
                printf(GREEN "==> Application de : \"%s\"\n" RESET, rules[i].action);
                applyRule(&currentState, &rules[i]);
                printf(CYAN "Nouveaux faits : " RESET);
                printState(&currentState);
                sep();
                ruleApplied = true;
                break;
            }
        }
        if (!ruleApplied) {
            printf(RED "Aucune règle applicable et l'objectif n'est pas atteint.\n" RESET);
            printf(RED "Le moteur pas très futé est bloqué...\n" RESET);
            return 1;
        }
    }

    printf(GREEN BOLD "Objectif atteint !\n" RESET);
    printf(BOLD "État final : " RESET);
    printState(&currentState);
    return 0;
}
