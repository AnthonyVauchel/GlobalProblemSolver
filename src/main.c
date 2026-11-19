#include "GPS.h"

int main(int argc, char *argv[]){
    bool quiet = false;
    bool measureTime = false;
    char *filePath = NULL;
    clock_t startTime = 0, endTime = 0;
    double elapsedTime = 0.0;
    
    for (int i = 1; i < argc; i++){
        if (strcmp(argv[i], "-q") == 0){
            quiet = true;
        } else if (strcmp(argv[i], "-t") == 0){
            measureTime = true;
        } else if (argv[i][0] == '-'){
            printf(RED "Options possibles :\n -q : utilisation silencieuse\n -t : mesurer le temps d'exécution\n" RESET);
            return 1;
        } else if (filePath == NULL){
            filePath = argv[i];
        } else {
            printf(RED "Usage : ./gps <options> <path>\n" RESET);
            return 1;
        }
    }

    if (filePath == NULL){
        printf(RED "Usage : ./gps <options> <path>\n" RESET);
        return 1;
    }
    
    trim(filePath);
    
    //Lecture et parsing du fichier
    if (!loadProblem(filePath)){
        printf("Erreur : impossible de charger le problème\n");
        return 1;
    }
    
    //Affichage du problème chargé (états de départ et d'arrivée)
    if (!quiet){
        sep();
        printf(BOLD "Start: " RESET);
        printState(&startState);
        printf(BOLD "Finish: " RESET);
        printState(&finishState);
        printf(BOLD "Règles chargées : " RESET "%d\n", ruleCount);
        sep();
    }
    
    //Démarrage du chronométrage si demandé
    if (measureTime){
        startTime = clock();
    }
    
    //Préparation du chemin initial de résolution (le chemin commence par l'état de départ)
    State path[MAX_DEPTH+1];
    path[0] = startState;
    
    //Lancement du backtracking en partant de la règle 0
    solve(path, 0, quiet);
    
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
