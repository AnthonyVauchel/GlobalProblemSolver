#include "GPS.h"

void sep(){
    //Affiche une ligne de séparation graphique
    printf("\n");
    for (int i = 0; i < 70; i++){
        printf("-");
    }
    printf("\n\n");
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