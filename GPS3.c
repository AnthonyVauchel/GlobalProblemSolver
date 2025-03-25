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
State startState;
State finishState;
Rule rules[MAX_RULES];
int ruleCount = 0;

bool solutionFound = false;      //Indique qu'une solution a été trouvée
char solutionActions[MAX_DEPTH][MAX_ACTION_LENGTH];
State solutionPath[MAX_DEPTH+1]; //solutionPath[i] correspond à l'état après i actions
int solutionLength = 0;          //Nombre d'actions dans la solution




//-----------------------------------------------------------------------------
//Fonctions utilitaires
//-----------------------------------------------------------------------------

void sep(){
    //Affiche une ligne de séparation graphique composée de 70 tirets
    printf("\n");
    for (int i = 0; i < 70; i++){
        printf("-");
    }
    printf("\n\n");
}

void trimNewline(char* s){
    //Supprime le caractère de saut de ligne ('\n') à la fin d'une chaîne si présent
    //Paramètre : pointeur sur la chaîne à traiter
    char* p = strchr(s, '\n');
    if (p){
        *p = '\0';
    }
}

void trimTrailingComma(char* s){
    //Supprime la virgule finale d'une chaîne si elle est présente
    //Paramètre : pointeur sur la chaîne à traiter
    int len = strlen(s);
    if (len > 0 && s[len-1] == ','){
        s[len-1] = '\0';
    }
}




//-----------------------------------------------------------------------------
//Fonction de parsing
//-----------------------------------------------------------------------------

int parseLine(char source[], string cible[]){
    /*Analyse une ligne de texte de la forme "clé: mot1, mot2, ..." et extrait 
    les mots (tokens) situés après le deux-points, chaque token est copié dans le tableau cible
    
    Fonctionnement : 
     -Recherche le caractère ':' dans la chaîne source
     -Ignore les espaces après le ':'
     -Découpe la chaîne en tokens en utilisant la virgule comme séparateur
     -Copie chaque token (enlevant les espaces en début et fin) dans le tableau cible    
     
    Paramètres :
     -source : la chaîne de caractères à analyser
     -cible : un tableau de type string dans lequel seront stockés les tokens extraits
     
    Retourne le nombre de tokens extraits*/

    int i = 0, n = 0;
    
    //Avancer jusqu'au ':' (si présent)
    while(source[i] != ':' && source[i] != '\0'){
        i++;
    }
    if (source[i] == '\0'){
        return 0; //Pas de ':' trouvé, aucun token n'est extrait
    }
    i++; //Passer le ':'
    
    //Sauter les espaces initiaux après le ':'
    while(source[i] == ' '){
        i++;
    }
    
    int j = i; //Marquer le début du premier mot
    while(source[i] != '\n' && source[i] != '\0'){
        if(source[i] == ','){
            int len = i -j;
            memcpy(cible[n], &source[j], len);
            cible[n][len] = '\0'; //Terminer la chaîne token
            n++;
            i++; //Passer la virgule
            
            //Ignorer les espaces après la virgule
            while(source[i] == ' '){
                i++;
            }
            j = i; //Définir le début du mot suivant
            continue;
        }
        i++;
    }
    
    //Copier le dernier token s'il y a quelque chose (avant le '\n' ou la fin de la chaîne)
    if (i > j){
        int len = i -j;
        memcpy(cible[n], &source[j], len);
        cible[n][len] = '\0';
        n++;
    }
    
    return n;
}




//-----------------------------------------------------------------------------
//Fonctions de gestion des faits
//-----------------------------------------------------------------------------

bool containsFact(State* s, const char* fact){
    /*Vérifie si un fait est présent dans un état
    
    Paramètres :
     -s : pointeur vers l'état à vérifier
     -fact : le fait à rechercher

    Retourne true si le fait est présent, false sinon*/

    for (int i = 0; i < s->factCount; i++){
        if (strcmp(s->facts[i], fact) == 0){
            return true;
        }
    }
    return false;
}

void addFact(State* s, const char* fact){
    /*Ajoute un fait à l'état s'il n'est pas déjà présent et si l'état n'est pas plein

    Paramètres :
     -s : pointeur vers l'état dans lequel ajouter le fait
     -fact : le fait à ajouter 
    */

    if (!containsFact(s, fact) && s->factCount < MAX_FACTS){
        strcpy(s->facts[s->factCount], fact);
        s->factCount++;
    }
}

void removeFact(State* s, const char* fact){
    /*Supprime un fait donné d'un état en décalant les autres faits pour combler l'espace

    Paramètres :
     -s : pointeur vers l'état dont le fait sera supprimé
     -fact : le fait à supprimer
    */

    for (int i = 0; i < s->factCount; i++){
        if (strcmp(s->facts[i], fact) == 0){
            //Décaler les faits suivants pour écraser le fait supprimé
            for (int j = i; j < s->factCount -1; j++){
                strcpy(s->facts[j], s->facts[j+1]);
            }
            s->factCount--;
            break;
        }
    }
}

bool goalReached(State* s, State* goal){
    /*Vérifie si l'état courant contient tous les faits du but
    
    Paramètres :
     -s : pointeur vers l'état courant
     -goal : pointeur vers l'état objectif (but)

    Retourne true si tous les faits du but sont présents dans l'état courant, false sinon*/

    for (int i = 0; i < goal->factCount; i++){
        if (!containsFact(s, goal->facts[i])){
            return false;
        }
    }
    return true;
}


bool ruleApplicable(State* s, Rule* r){
    /*Teste si une règle est applicable à un état, c'est-à-dire si toutes ses préconditions sont satisfaites dans l'état courant

    Paramètres :
     -s : pointeur vers l'état courant
     -r : pointeur vers la règle à tester

    Retourne true si la règle est applicable, false sinon*/

    for (int i = 0; i < r->precondCount; i++){
        if (!containsFact(s, r->preconds[i])){
            return false;
        }
    }
    return true;
}

void applyRule(State* s, Rule* r, State* newState){
    /* Applique une règle à un état courant pour produire un nouvel état
    Le nouvel état est obtenu en supprimant les faits spécifiés dans r->del puis en ajoutant ceux spécifiés dans r->add

    Paramètres :
    -s : pointeur vers l'état courant
    -r : pointeur vers la règle à appliquer
    -newState : pointeur vers l'état où sera stocké le résultat
    */

    //Copier l'état courant dans newState
    *newState = *s;
    
    //Supprimer les faits indiqués dans la partie "delete" de la règle
    for (int i = 0; i < r->delCount; i++){
        removeFact(newState, r->del[i]);
    }
    
    //Ajouter les nouveaux faits indiqués dans la partie "add" de la règle
    for (int i = 0; i < r->addCount; i++){
        addFact(newState, r->add[i]);
    }
}

bool stateEquals(State* s1, State* s2){
    /*Compare deux états pour vérifier s'ils sont identiques (même nombre de faits et mêmes faits présents)

    Paramètres :
    -s1 : pointeur vers le premier état
    -s2 : pointeur vers le deuxième état

    Retourne : true si les deux états sont identiques, false sinon*/

    if (s1->factCount != s2->factCount){
        return false;
    }
    
    for (int i = 0; i < s1->factCount; i++){
        if (!containsFact(s2, s1->facts[i])){
            return false;
        }
    }
    return true;
}

bool stateInPath(State path[], int depth, State* s){
    /*Vérifie si un état a déjà été rencontré dans le chemin courant afin d'éviter les cycles dans l'exploration

    Paramètres :
    -path  : tableau représentant le chemin (série d'états déjà visités)
    -depth : profondeur actuelle (nombre d'états dans le chemin)
    -s     : pointeur vers l'état à rechercher

    Retourne : true si l'état est déjà présent dans le chemin, false sinon*/

    for (int i = 0; i < depth; i++){
        if (stateEquals(&path[i], s)){
            return true;
        }
    }
    return false;
}


void printState(State* s){
    /*Affiche l'état (la liste des faits) en utilisant une couleur.
    Paramètre : pointeur vers l'état à afficher*/

    printf(YELLOW);
    for (int i = 0; i < s->factCount; i++){
        printf("\"%s\"", s->facts[i]);
        if (i < s->factCount -1){
            printf(", ");
        }
    }
    printf(RESET "\n");
}

void printSolution(){
    // Affiche la solution finale : le nombre d'étapes, l'état final et la liste des actions à réaliser

    printf(BOLD "\nNombre d'étapes : %d\n" RESET, solutionLength);
    
    //Afficher l'état final
    printf(BLUE "\nEn conclusion, on a : " RESET);
    printState(&solutionPath[solutionLength]);
    
    printf(GREEN "\nL'objectif est atteint !\n" RESET);
    
    //Afficher la séquence d'actions menant à la solution
    printf(BOLD "\nLes actions à faire sont : " RESET);
    for (int i = 0; i < solutionLength; i++){
        printf("\"%s\"", solutionActions[i]);
        if (i < solutionLength -1){
            printf(", ");
        }
    }
    printf("\n");
}




//-----------------------------------------------------------------------------
//Fonction de résolution avec backtracking (utilisation de startRule)
//-----------------------------------------------------------------------------




void solve(State current, State path[], int depth, int startRule){
    /*Fonction récursive qui explore l'espace des états en appliquant les règles pour atteindre le but en utilisant un mécanisme de backtracking

    Fonctionnement :
    -Si l'état courant satisfait le but, la solution est mémorisée et la recherche s'arrête
    -Sinon, la fonction parcourt toutes les règles applicables à partir de startRule
    -Pour chaque règle applicable qui modifie l'état, la fonction l'applique pour obtenir un nouvel état et appelle récursivement solve pour continuer la recherche à partir de cet état
    -Si aucune règle n'est applicable, la fonction revient en arrière (backtrack)

    Paramètres :
    -current   : l'état courant
    -path      : tableau mémorisant le chemin parcouru (les états successifs)
    -depth     : profondeur actuelle (nombre d'étapes effectuées)
    -startRule : indice à partir duquel les règles sont testées (permet de reprendre après un backtrack)
    */

    if (solutionFound) return;
    if (depth > MAX_DEPTH) return;
    
    //Si l'état courant satisfait le but, mémoriser le chemin de solution
    if (goalReached(&current, &finishState)){
        solutionFound = true;
        for (int i = 0; i <= depth; i++){
            solutionPath[i] = path[i];
        }
        solutionLength = depth;
        return;
    }
    
    bool appliedAnyRule = false;
    
    //Parcourir les règles applicables à partir de l'indice startRule
    for (int i = startRule; i < ruleCount; i++){
        if (ruleApplicable(&current, &rules[i])){
            appliedAnyRule = true;
            
            State next;
            applyRule(&current, &rules[i], &next);
            
            //Si l'application de la règle ne modifie pas l'état, on passe à la suivante
            if (stateEquals(&current, &next)){
                printf(RED "\nAction \"%s\" ne modifie pas l'état. On la saute.\n" RESET, rules[i].action);
                continue;
            }
            
            //Si le nouvel état n'est pas déjà dans le chemin (pour éviter les cycles)
            if (!stateInPath(path, depth + 1, &next)){
                strcpy(solutionActions[depth], rules[i].action);
                printf(GREEN "\nOn réalise : %s\n" RESET, rules[i].action);
                printf(BLUE "Les états actuels sont maintenant : " RESET);
                printState(&next);
                
                path[depth + 1] = next;
                //Pour le nouvel état, réinitialiser l'indice des règles (startRule = 0)
                solve(next, path, depth + 1, 0);
                
                if (solutionFound) return;
                
                printf(RED "\nOn revient en arrière après avoir tenté : %s\n" RESET, rules[i].action);
            }
        }
    }
    
    //Si aucune règle applicable n'a été trouvée, indiquer le retour en arrière
    if (!appliedAnyRule){
        printf(RED "Aucune règle applicable, on revient en arrière.\n" RESET);
    }
}




//-----------------------------------------------------------------------------
//Fonction principale
//-----------------------------------------------------------------------------




int main(){
    /*Point d'entrée du programme

    Fonctionnement :
     -Demande le chemin vers le fichier contenant le problème
     -Lit et parse le fichier pour initialiser l'état de départ, l'état but et les règles
     -Demande si l'utilisateur souhaite chronométrer la résolution
     -Lance la résolution par backtracking pour trouver une séquence d'actions menant au but
     -Affiche la solution trouvée (ou un message d'erreur si aucune solution n'est trouvée), ainsi que le temps de résolution si demandé
    */

    char filePath[256];
    char chronoChoice[10];
    bool measureTime = false;
    clock_t startTime, endTime;
    double elapsedTime = 0.0;
    
    //Demande du chemin vers le fichier du problème
    printf(BOLD "Veuillez entrer le chemin vers le fichier du problème : " RESET);
    if (!fgets(filePath, sizeof(filePath), stdin)){
        printf(RED "Erreur lors de la lecture du chemin.\n" RESET);
        return 1;
    }
    trimNewline(filePath);
    
    //Ouverture du fichier
    FILE* file = fopen(filePath, "r");
    if (!file){
        printf(RED "Erreur : impossible d'ouvrir le fichier '%s'\n" RESET, filePath);
        return 1;
    }
    
    //Demande si l'utilisateur souhaite chronométrer la résolution
    printf(BOLD "Souhaitez-vous chronométrer le temps de résolution ? (O/N) : " RESET);
    if (fgets(chronoChoice, sizeof(chronoChoice), stdin)){
        if (chronoChoice[0] == 'O' || chronoChoice[0] == 'o'){
            measureTime = true;
        }
    }
    
    //Lecture et parsing du fichier ligne par ligne
    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, file)){
        trimNewline(line);
        if (strlen(line) == 0) continue;
        if (strcmp(line, "****") == 0) continue;
        
        //Traitement de la ligne selon son préfixe ("start:", "finish:", "action:")
        if (strncmp(line, "start:", 6) == 0){
            string tokens[MAX_FACTS];
            int n = parseLine(line, tokens);
            startState.factCount = 0;
            for (int i = 0; i < n; i++){
                addFact(&startState, tokens[i]);
            }
        }
        else if (strncmp(line, "finish:", 7) == 0){
            string tokens[MAX_FACTS];
            int n = parseLine(line, tokens);
            finishState.factCount = 0;
            for (int i = 0; i < n; i++){
                addFact(&finishState, tokens[i]);
            }
        }
        else if (strncmp(line, "action:", 7) == 0){
            Rule r;
            string tokens[MAX_FACTS];
            int n = parseLine(line, tokens);
            
            //Récupération de l'action depuis le token
            if (n > 0){
                strcpy(r.action, tokens[0]);
            }
            else{
                //Si aucun token n'est extrait, copier le reste de la ligne après le ':'
                char* colon = strchr(line, ':');
                if (colon){
                    colon++; //passer le ':'
                    while (*colon == ' '){
                        colon++;
                    }
                    strcpy(r.action, colon);
                    char* nl = strchr(r.action, '\n');
                    if (nl){
                        *nl = '\0';
                    }
                }
            }
            
            //Lecture des préconditions
            if (fgets(line, MAX_LINE_LENGTH, file)){
                string tokens2[MAX_FACTS];
                int n2 = parseLine(line, tokens2);
                r.precondCount = n2;
                for (int i = 0; i < n2; i++){
                    strcpy(r.preconds[i], tokens2[i]);
                }
            }
            
            //Lecture des faits à ajouter
            if (fgets(line, MAX_LINE_LENGTH, file)){
                string tokens3[MAX_FACTS];
                int n3 = parseLine(line, tokens3);
                r.addCount = n3;
                for (int i = 0; i < n3; i++){
                    strcpy(r.add[i], tokens3[i]);
                }
            }
            
            //Lecture des faits à supprimer
            if (fgets(line, MAX_LINE_LENGTH, file)){
                string tokens4[MAX_FACTS];
                int n4 = parseLine(line, tokens4);
                r.delCount = n4;
                for (int i = 0; i < n4; i++){
                    strcpy(r.del[i], tokens4[i]);
                }
            }
            
            //Stockage de la règle dans le tableau global
            rules[ruleCount++] = r;
        }
    }
    fclose(file);
    
    //Affichage du problème chargé (états de départ et d'arrivée)
    sep();
    printf(BOLD "Start: " RESET);
    for (int i = 0; i < startState.factCount; i++){
        printf("\"%s\"", startState.facts[i]);
        if (i < startState.factCount -1){
            printf(", ");
        }
    }
    printf("\n" BOLD "Finish: " RESET);
    for (int i = 0; i < finishState.factCount; i++){
        printf("\"%s\"", finishState.facts[i]);
        if (i < finishState.factCount -1){
            printf(", ");
        }
    }
    printf("\n");
    sep();
    
    //Démarrage du chronométrage si demandé
    if (measureTime){
        startTime = clock();
    }
    
    //Préparation du chemin initial de résolution (le chemin commence par l'état de départ)
    State path[MAX_DEPTH+1];
    path[0] = startState;
    
    //Lancement du backtracking en partant de la règle 0
    solve(startState, path, 0, 0);
    
    //Arrêt du chronométrage si demandé
    if (measureTime){
        endTime = clock();
        elapsedTime = (double)(endTime -startTime) / CLOCKS_PER_SEC;
    }
    
    //Affichage final de la solution ou d'un message indiquant qu'aucune solution n'a été trouvée
    if (solutionFound){
        printSolution();
    }
    else{
        printf(RED "\nAucune solution trouvée.\n" RESET);
    }
    
    if (measureTime){
        printf(MAGENTA "\nTemps de résolution : %.3f secondes\n" RESET, elapsedTime);
    }
    
    return 0;
}
