#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FACTS 100
#define MAX_FACT_LENGTH 50
#define MAX_RULES 100
#define MAX_ACTION_LENGTH 100
#define MAX_LINE_LENGTH 256
#define MAX_PRECONDS 10
#define MAX_ADD 10
#define MAX_DELETE 10

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define BOLD    "\033[1m"

typedef char string[50];

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

State startState;
State goalState;
Rule rules[MAX_RULES];
int ruleCount = 0;

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
    while (source[i] != '\0') {
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

void addFactsToState(State* s, string facts[], int count) {
    for (int i = 0; i < count; i++) {
        strcpy(s->facts[s->factCount++], facts[i]);
    }
}

void sep() {
    printf("\n");
    for (int i = 0; i < 70; i++) printf("-");
    printf("\n\n");
}

void printFacts(char label[], State* s) {
    printf(BOLD "%s" RESET, label);
    for (int i = 0; i < s->factCount; i++) {
        printf("%s", s->facts[i]);
        if (i < s->factCount - 1) printf(", ");
    }
    printf("\n");
}

void printRule(Rule* r) {
    printf(BOLD "Action : " RESET "%s\n", r->action);

    printf(CYAN "  Préconditions : " RESET);
    for (int i = 0; i < r->precondCount; i++) {
        printf("%s", r->preconds[i]);
        if (i < r->precondCount - 1) printf(", ");
    }
    if (r->precondCount == 0) printf("(aucune)");
    printf("\n");

    printf(GREEN "  Ajouts : " RESET);
    for (int i = 0; i < r->addCount; i++) {
        printf("%s", r->add[i]);
        if (i < r->addCount - 1) printf(", ");
    }
    if (r->addCount == 0) printf("(aucun)");
    printf("\n");

    printf(RED "  Suppressions : " RESET);
    for (int i = 0; i < r->delCount; i++) {
        printf("%s", r->del[i]);
        if (i < r->delCount - 1) printf(", ");
    }
    if (r->delCount == 0) printf("(aucune)");
    printf("\n\n");
}

int main() {
    char filePath[256];
    printf(BOLD "Veuillez entrer le chemin vers le fichier du problème : " RESET);
    if (!fgets(filePath, sizeof(filePath), stdin)) {
        printf(RED "Erreur de lecture du chemin.\n" RESET);
        return 1;
    }
    trimNewline(filePath);

    FILE* file = fopen(filePath, "r");
    if (!file) {
        printf(RED "Erreur d'ouverture du fichier.\n" RESET);
        return 1;
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, file)) {
        trimNewline(line);
        if (strlen(line) == 0 || strcmp(line, "****") == 0) continue;

        if (strncmp(line, "start:", 6) == 0) {
            string tokens[MAX_FACTS];
            int n = parseLine(line, tokens);
            startState.factCount = 0;
            addFactsToState(&startState, tokens, n);
        } else if (strncmp(line, "finish:", 7) == 0) {
            string tokens[MAX_FACTS];
            int n = parseLine(line, tokens);
            goalState.factCount = 0;
            addFactsToState(&goalState, tokens, n);
        } else if (strncmp(line, "action:", 7) == 0) {
            Rule r;
            string tokens[MAX_FACTS];
            int n = parseLine(line, tokens);
            strcpy(r.action, tokens[0]);

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
    printFacts("Start: ", &startState);
    printFacts("Finish: ", &goalState);
    sep();

    printf(BOLD "Règles chargées :\n\n" RESET);
    for (int i = 0; i < ruleCount; i++) {
        printRule(&rules[i]);
    }

    return 0;
}
