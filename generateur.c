#include <stdio.h>
#include <stdlib.h>

void generate_rules(int num_blocks, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    //Écrire la configuration initiale
    fprintf(file, "start:space on a,");
    for (int i = 0; i < num_blocks; ++i) {
        if (i == 0) {
            fprintf(file, "a on b,");
        } else if (i == num_blocks - 1) {
            fprintf(file, "%c on table,", 'a' + i);
        } else {
            fprintf(file, "%c on %c,", 'a' + i, 'a' + i + 1);
        }
    }
    fprintf(file, "space on table,\n");

    //Écrire la configuration finale
    fprintf(file, "finish:space on %c,", 'a' + num_blocks - 1);
    for (int i = num_blocks - 1; i >= 0; --i) {
        if (i == num_blocks - 1) {
            fprintf(file, "%c on %c,", 'a' + i, 'a' + i - 1);
        } else if (i == 0) {
            fprintf(file, "%c on table,", 'a' + i);
        } else {
            fprintf(file, "%c on %c,", 'a' + i, 'a' + i - 1);
        }
    }
    fprintf(file, "space on table,\n");

    //Générer les actions dans l'ordre spécifique
    for (char block = 'a'; block < 'a' + num_blocks; ++block) {
        //1. Déplacer d'un bloc à un autre
        for (char from = 'a'; from < 'a' + num_blocks; ++from) {
            if (from == block) continue;
            for (char to = 'a'; to < 'a' + num_blocks; ++to) {
                if (to == block || to == from) continue;

                //Écrire l'action
                fprintf(file, "****\n");
                fprintf(file, "action:move %c from %c to %c,\n", block, from, to);
                fprintf(file, "preconds:space on %c,space on %c,%c on %c,\n", block, to, block, from);
                fprintf(file, "add:%c on %c,space on %c,\n", block, to, from);
                fprintf(file, "delete:%c on %c,space on %c,\n", block, from, to);
            }
        }

        //2. Déplacer de la table à un autre bloc
        for (char to = 'a'; to < 'a' + num_blocks; ++to) {
            if (to == block) continue;

            //Écrire l'action
            fprintf(file, "****\n");
            fprintf(file, "action:move %c from table to %c,\n", block, to);
            fprintf(file, "preconds:space on %c,space on %c,%c on table,\n", block, to, block);
            fprintf(file, "add:%c on %c,\n", block, to);
            fprintf(file, "delete:%c on table,space on %c,\n", block, to);
        }

        //3. Déplacer d'un autre bloc à la table
        for (char from = 'a'; from < 'a' + num_blocks; ++from) {
            if (from == block) continue;

            //Écrire l'action
            fprintf(file, "****\n");
            fprintf(file, "action:move %c from %c to table,\n", block, from);
            fprintf(file, "preconds:space on %c,space on table,%c on %c,\n", block, block, from);
            fprintf(file, "add:%c on table,space on %c,\n", block, from);
            fprintf(file, "delete:%c on %c,\n", block, from);
        }
    }

    fclose(file);
}

int main() {
    int num_blocks;

    //Demander à l'utilisateur le nombre de blocs
    printf("Entrez le nombre de blocs (2 ou plus) : ");
    scanf("%d", &num_blocks);

    if (num_blocks < 2) {
        printf("Le nombre de blocs doit être au moins 2.\n");
        return 1;
    }

    //Générer le nom du fichier en fonction du nombre de blocs
    char filename[50];
    snprintf(filename, sizeof(filename), "blocs_%d.txt", num_blocks);

    //Générer les règles dans le fichier
    generate_rules(num_blocks, filename);

    printf("Fichier de règles généré avec succès : %s\n", filename);

    return 0;
}