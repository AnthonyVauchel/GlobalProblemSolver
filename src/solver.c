#include "GPS.h"

bool solutionFound = false;
char solutionActions[MAX_DEPTH][MAX_ACTION_LENGTH];
State solutionPath[MAX_DEPTH + 1];
int solutionLength = 0;

void solve(State path[], int depth){
    /*Fonction récursive qui explore l'espace des états en appliquant les règles pour atteindre le but en utilisant un mécanisme de backtracking

    Fonctionnement :
    -Si l'état courant satisfait le but, la solution est mémorisée et la recherche s'arrête
    -Sinon, la fonction parcourt toutes les règles applicables à partir de startRule
    -Pour chaque règle applicable qui modifie l'état, la fonction l'applique pour obtenir un nouvel état et appelle récursivement solve pour continuer la recherche à partir de cet état
    -Si aucune règle n'est applicable, la fonction revient en arrière (backtrack)

    Paramètres :
    -current : l'état courant
    -path : tableau mémorisant le chemin parcouru (les états successifs)
    -depth : profondeur actuelle (nombre d'étapes effectuées)
    -startRule : indice à partir duquel les règles sont testées (permet de reprendre après un backtrack)
    */

    if (solutionFound) return;
    
    State *current = &path[depth];
    
    //Si l'état courant satisfait le but, mémoriser le chemin de solution
    if (goalReached(current, &finishState)){
        solutionFound = true;
        for (int i = 0; i <= depth; i++){
            solutionPath[i] = path[i];
        }
        solutionLength = depth;
        return;
    }
    
    if (depth >= MAX_DEPTH) return;

    State *next = &path[depth+1];
    
    //Parcourir les règles applicables à partir de l'indice startRule
    for (int i = 0; i < ruleCount; i++){
        if (ruleApplicable(current, &rules[i])){
            
            applyRule(current, &rules[i], next);
            
            //Si l'application de la règle ne modifie pas l'état, on passe à la suivante
            if (stateEquals(current, next)){
                printf(RED "\nAction \"%s\" ne modifie pas l'état. On la saute.\n" RESET, rules[i].action);
                continue;
            }
            
            //Si le nouvel état n'est pas déjà dans le chemin (pour éviter les cycles)
            if (!stateInPath(path, depth + 1, next)){
                strcpy(solutionActions[depth], rules[i].action);
                printf(GREEN "\nOn réalise : %s\n" RESET, rules[i].action);
                printf(BLUE "Les états actuels sont maintenant : " RESET);
                printState(next);
                
                //Pour le nouvel état, réinitialiser l'indice des règles (startRule = 0)
                solve(path, depth + 1);
                if (solutionFound) return;
                
                printf(RED "\nOn revient en arrière après avoir tenté : %s\n" RESET, rules[i].action);
            }
        }
    }
    
    //Si aucune règle applicable n'a été trouvée, indiquer le retour en arrière
    printf(RED "Aucune règle applicable, on revient en arrière.\n" RESET);
}

bool stateInPath(State path[], int depth, State* s){
    /*Vérifie si un état a déjà été rencontré dans le chemin courant afin d'éviter les cycles dans l'exploration

    Paramètres :
    -path : tableau représentant le chemin (série d'états déjà visités)
    -depth : profondeur actuelle (nombre d'états dans le chemin)
    -s : pointeur vers l'état à rechercher

    Retourne : true si l'état est déjà présent dans le chemin, false sinon*/

    for (int i = 0; i < depth; i++){
        if (stateEquals(&path[i], s)){
            return true;
        }
    }
    return false;
}
