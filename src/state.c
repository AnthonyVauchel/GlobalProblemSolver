#include "GPS.h"

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