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

/*
 * Fonction principale :
 *   - Lit le chemin du fichier à traiter.
 *   - Lit le start et le finish et les affiche.
 *   - Parcourt ensuite le fichier pour lire et stocker chaque règle.
 */
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

    // Affichage du start.
    printf("\nStart: ");
    for (int i = 0; i < nbStart; i++) {
        printf("%s", startTokens[i]);
        if (i < nbStart - 1)
            printf(", ");
    }
    printf("\n");

    // Affichage du finish.
    printf("Finish: ");
    for (int i = 0; i < nbFinish; i++) {
        printf("%s", finishTokens[i]);
        if (i < nbFinish - 1)
            printf(", ");
    }
    printf("\n\n");

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

    

    return 0;
}
