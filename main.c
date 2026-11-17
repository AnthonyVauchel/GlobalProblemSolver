#include "GPS.h"

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
