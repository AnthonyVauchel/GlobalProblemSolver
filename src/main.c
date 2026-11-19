#include "GPS.h"

int main(){

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
    trim(filePath);
    
    //Lecture et parsing du fichier
    if (!loadProblem(filePath)){
        printf("Erreur : impossible de charger le problème\n");
        return 1;
    }
    
    //Demande si l'utilisateur souhaite chronométrer la résolution
    printf(BOLD "Souhaitez-vous chronométrer le temps de résolution ? (O/N) : " RESET);
    if (fgets(chronoChoice, sizeof(chronoChoice), stdin)){
        if (chronoChoice[0] == 'O' || chronoChoice[0] == 'o'){
            measureTime = true;
        }
    }
    
    //Affichage du problème chargé (états de départ et d'arrivée)
    sep();
    printf(BOLD "Start: " RESET);
    printState(&startState);
    printf(BOLD "Finish: " RESET);
    printState(&finishState);
    printf(BOLD "Règles chargées : " RESET "%d\n", ruleCount);
    sep();
    
    //Démarrage du chronométrage si demandé
    if (measureTime){
        startTime = clock();
    }
    
    //Préparation du chemin initial de résolution (le chemin commence par l'état de départ)
    State path[MAX_DEPTH+1];
    path[0] = startState;
    
    //Lancement du backtracking en partant de la règle 0
    solve(path, 0);
    
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
    
    return solutionFound ? 0 : 1;
}
