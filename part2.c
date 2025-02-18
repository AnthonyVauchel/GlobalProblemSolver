#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Définition d'un type chaîne de caractères de taille fixe
typedef char string[200];

// Structure pour stocker une règle
typedef struct Regles {
    string action;
    string preconds[20];
    string add[20];
    string delete[20];
    int nbPreconds;
    int nbAdd;
    int nbDelete;
} Regles;

// Tableaux globaux pour les règles, le start et le finish
Regles regles[256];
int nbRegles = 0;
string startTokens[20];
int nbStart = 0;
string finishTokens[20];
int nbFinish = 0;

/*
 * Fonction splitligne :
 *   - Recherche le caractère ':' dans la ligne et découpe la partie qui suit en tokens,
 *     en utilisant la virgule (',') et le saut de ligne ('\n') comme délimiteurs.
 *   - Les tokens obtenus sont stockés dans le tableau list.
 *   - La fonction retourne le nombre de tokens extraits.
 */
int splitligne(char ligne[], string list[], int len) {
    // Recherche du caractère ':' dans la ligne.
    char *colon = strchr(ligne, ':');
    if (!colon)
        return 0;  // Aucun deux-points trouvé.

    // On passe le deux-points pour travailler sur la partie utile.
    char *data = colon + 1;
    int count = 0;

    // Découpage de la chaîne à l'aide de strtok.
    char *token = strtok(data, ",\n");
    while (token != NULL && count < len) {
        // Éliminer d'éventuels espaces en début de token.
        while (*token == ' ' || *token == '\t')
            token++;
        strcpy(list[count], token);
        count++;
        token = strtok(NULL, ",\n");
    }
    return count;
}

//---------------------Fonctions pour le raisonnement par chainage avant---------------------

// Vérifie si un fait est déjà présent dans l'ensemble des faits courants.
int factExists(const char fact[], string facts[], int nbFacts) {
    for (int i = 0; i < nbFacts; i++) {
        if (strcmp(fact, facts[i]) == 0)
            return 1;
    }
    return 0;
}

// Vérifie si tous les faits du but sont présents dans les faits courants.
int containsAllGoals(string currentFacts[], int nbCurrentFacts, string goals[], int nbGoals) {
    for (int i = 0; i < nbGoals; i++) {
        if (!factExists(goals[i], currentFacts, nbCurrentFacts))
            return 0;
    }
    return 1;
}

// Vérifie si une règle est applicable, c'est-à-dire si toutes ses préconditions
// se trouvent dans l'ensemble des faits courants.
int ruleIsApplicable(Regles r, string currentFacts[], int nbCurrentFacts) {
    for (int i = 0; i < r.nbPreconds; i++) {
        if (!factExists(r.preconds[i], currentFacts, nbCurrentFacts))
            return 0;
    }
    return 1;
}

// Ajoute un fait dans l'ensemble des faits courants s'il n'est pas déjà présent.
void addFact(const char fact[], string currentFacts[], int *nbCurrentFacts) {
    if (!factExists(fact, currentFacts, *nbCurrentFacts)) {
        strcpy(currentFacts[*nbCurrentFacts], fact);
        (*nbCurrentFacts)++;
        printf("  Ajout du fait: %s\n", fact);
    }
}

// Supprime un fait de l'ensemble des faits courants s'il est présent.
void removeFact(const char fact[], string currentFacts[], int *nbCurrentFacts) {
    for (int i = 0; i < *nbCurrentFacts; i++) {
        if (strcmp(currentFacts[i], fact) == 0) {
            printf("  Suppression du fait: %s\n", fact);
            // On remplace le fait à supprimer par le dernier fait du tableau
            strcpy(currentFacts[i], currentFacts[*nbCurrentFacts - 1]);
            (*nbCurrentFacts)--;
            // On décrémente i pour vérifier la case déplacée
            i--;
        }
    }
}

//---------------------Fonctions main---------------------

int main(void) {
    char chemin[256];

    // Demander le chemin du fichier contenant les instructions/règles.
    printf("Chemin vers fichier avec instructions : ");
    scanf("%s", chemin);

    FILE *fichier = fopen(chemin, "r");
    if (fichier == NULL) {
        printf("Fichier introuvable.\n");
        exit(1);
    }

    char ligne[256];

    // Lecture et découpe de la ligne 'start:'.
    if (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        nbStart = splitligne(ligne, startTokens, 20);
    } else {
        printf("Erreur lors de la lecture du start.\n");
        exit(1);
    }

    // Lecture et découpe de la ligne 'finish:'.
    if (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        nbFinish = splitligne(ligne, finishTokens, 20);
    } else {
        printf("Erreur lors de la lecture du finish.\n");
        exit(1);
    }


    /*
     * Parcours du reste du fichier pour lire les règles.
     * On ignore les lignes contenant uniquement "****".
     * On s'attend à trouver une ligne commençant par "action:" suivie
     * des trois lignes suivantes (preconds, add et delete).
     */
    while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        // Retirer le saut de ligne en fin de chaîne.
        char *pos;
        if ((pos = strchr(ligne, '\n')) != NULL)
            *pos = '\0';

        // Ignorer les lignes vides.
        if (strlen(ligne) == 0)
            continue;

        // Si la ligne est un séparateur (par exemple "****"), on l'ignore.
        int index = 0;
        while (ligne[index] && (ligne[index] == ' ' || ligne[index] == '\t'))
            index++;
        if (ligne[index] == '*' )
            continue;

        // On s'attend à trouver une ligne commençant par "action:".
        if (strncmp(ligne, "action:", 7) == 0) {
            Regles r;

            // Traitement de la ligne d'action.
            {
                string tokenAction[1];
                splitligne(ligne, tokenAction, 1);
                strcpy(r.action, tokenAction[0]);
            }

            // Lecture et découpe de la ligne des préconditions.
            if (fgets(ligne, sizeof(ligne), fichier) == NULL)
                break;
            r.nbPreconds = splitligne(ligne, r.preconds, 20);

            // Lecture et découpe de la ligne des ajouts.
            if (fgets(ligne, sizeof(ligne), fichier) == NULL)
                break;
            r.nbAdd = splitligne(ligne, r.add, 20);

            // Lecture et découpe de la ligne des suppressions.
            if (fgets(ligne, sizeof(ligne), fichier) == NULL)
                break;
            r.nbDelete = splitligne(ligne, r.delete, 20);

            // Stockage de la règle dans le tableau global.
            regles[nbRegles++] = r;
        }
    }

    fclose(fichier);

    /* ==========================================
       MOTEUR DE RAISONNEMENT PAR CHAINAGE AVANT
       ========================================== */

    // Initialisation des faits courants avec le start.
    string currentFacts[256];
    int nbCurrentFacts = 0;
    for (int i = 0; i < nbStart; i++) {
        strcpy(currentFacts[nbCurrentFacts++], startTokens[i]);
    }

    int step = 1;
    int ruleApplied;
    printf("=== Démarrage du chainage avant ===\n");
    // Tant que tous les buts ne sont pas atteints...
    while (!containsAllGoals(currentFacts, nbCurrentFacts, finishTokens, nbFinish)) {
        ruleApplied = 0;
        printf("\nÉtape %d :\n", step);
        // Parcours de l'ensemble des règles pour trouver une règle applicable.
        for (int i = 0; i < nbRegles; i++) {
            if (ruleIsApplicable(regles[i], currentFacts, nbCurrentFacts)) {
                printf("Application de la règle: %s\n", regles[i].action);
                // Ajout des faits de type "add".
                for (int j = 0; j < regles[i].nbAdd; j++) {
                    addFact(regles[i].add[j], currentFacts, &nbCurrentFacts);
                }
                // Suppression des faits de type "delete".
                for (int j = 0; j < regles[i].nbDelete; j++) {
                    removeFact(regles[i].delete[j], currentFacts, &nbCurrentFacts);
                }
                ruleApplied = 1;
                break; // On applique une règle par étape.
            }
        }
        if (!ruleApplied) {
            printf("Aucune règle applicable. Le but ne peut être atteint.\n");
            break;
        }
        step++;
        // Affichage des faits courants après application de la règle.
        printf("Faits courants: ");
        for (int i = 0; i < nbCurrentFacts; i++) {
            printf("%s", currentFacts[i]);
            if (i < nbCurrentFacts - 1)
                printf(", ");
        }
        printf("\n");
    }

    if (containsAllGoals(currentFacts, nbCurrentFacts, finishTokens, nbFinish)){
        printf("\nBut atteint en %d étape(s) !\n", step - 1);
    }

    return 0;
}
