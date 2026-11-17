#include "GPS.h"

State startState;
State finishState;
Rule rules[MAX_RULES];
int ruleCount = 0;

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