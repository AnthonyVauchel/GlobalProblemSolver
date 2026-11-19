#include "GPS.h"

State startState;
State finishState;
Rule rules[MAX_RULES];
int ruleCount = 0;

void trim(char *s){

    //Suppression des caractères parasites en fin de chaîne
    int len = strlen(s);
    while (len > 0 && (s[len-1] == ' ' || s[len-1] == '\t' || s[len-1] == '\r' || s[len-1] == '\n')){
        s[--len] = '\0';
    }

    //Suppression des caractères en début de chaîne
    int start = 0;
    while (s[start] == ' ' || s[start] == '\t'){
        start++;
    }
    if (start > 0){
        memmove(s, s + start, len - start + 1);
    }
}

void parseError(int lineNo, const char* msg){
    //Affiche message d'erreur
    fprintf(stderr, RED "Erreur ligne %d : %s\n", lineNo, msg);
}

bool verifLine(FILE *file, char *buffer, int *lineNo){
    //Vérifie si la ligne a été coupée ou si on doit l'ignorer 

    while (fgets(buffer, MAX_LINE_LENGTH, file)){
        (*lineNo)++;

        //Si pas de saut de ligne dans s et que ça n'est pas la fin de fichier : ligne trop longue pour le buffer, elle a été coupée
        if (strchr(buffer, '\n') == NULL && !feof(file)){
            parseError(*lineNo, "ligne trop longue");
            return false;
        }

        trim(buffer);
        //Si ligne vide ou contient ****, on ignore
        if (buffer[0] == '\0' || strcmp(buffer, "****") == 0){
            continue;
        }
        return true;
    }
    return false;
}

int parseLine(const char *line, const char* key, string cible[], int maxTokens, int lineNo){
    //Vérifie que la ligne commence par key: et découpe la suite en tokens séparés par des virgules
    
    size_t keyLen = strlen(key);
    const char *p = line + keyLen + 1; //Juste après le :
    int n = 0;
    
    while (*p != '\0'){
        //Le token va jusqu'à la prochaine virgule ou jusqu'à la fin de la ligne
        char *end = strchr(p, ',');
        size_t len;

        if (end != NULL){
            len = (size_t)(end - p);
        } else {
            len = strlen(p);
        }

        if (len >= MAX_FACT_LENGTH){
            parseError(lineNo, "fait trop long");
            return -1;
        }

        char token[MAX_FACT_LENGTH];
        memcpy(token, p, len);
        token[len] = '\0';
        trim(token);

        if (token[0] != '\0'){
            if (n >= maxTokens){
                parseError(lineNo, "trop d'éléments sur cette ligne");
                return -1;
            }
            strcpy(cible[n], token);
            n++;
        }

        if (end == NULL){
            break;
        }
        p = end + 1; //On passe la virgule
    }
    return n;
}

bool parseState(const char *line, const char *key, State *s, int lineNo){
    //Remplit un état à partir d'une ligne start ou finish
    string tokens[MAX_FACTS];
    int n = parseLine(line, key, tokens, MAX_FACTS, lineNo);

    if (n < 0){
        return false;
    }

    s->factCount = 0;
    for (int i = 0; i < n; i++){
        addFact(s, tokens[i]);
    }

    return true;
}

bool parseAction(FILE *file, char *line, int *lineNo){
    //Lit un bloc de règle complet

    if (ruleCount >= MAX_RULES){
        parseError(*lineNo, "trop de règles");
        return false;
    }

    Rule r = {0};

    char name[MAX_LINE_LENGTH];
    strcpy(name, line + strlen("action:")); //On copie tout à partir de l'adresse line + taille de action:
    trim(name);
    int len = strlen(name);

    if (len > 0 && name[len-1] == ','){
        name[--len] = '\0';
        trim(name);
        len = strlen(name);
    }

    if (len == 0){
        parseError(*lineNo, "action sans nom");
        return false;
    }

    if (len >= MAX_ACTION_LENGTH){
        parseError(*lineNo, "nom d'action trop long");
        return false;
    }

    strcpy(r.action, name);

    //Ajout des 3 lignes suivantes
    char next[MAX_LINE_LENGTH];
    if (!verifLine(file, next, lineNo)){
        parseError(*lineNo, "bloc action incomplet (preconds manquant)");
        return false;
    }
    r.precondCount = parseLine(next, "preconds", r.preconds, MAX_PRECONDS, *lineNo);
    if (r.precondCount < 0) return false;
 
    if (!verifLine(file, next, lineNo)){
        parseError(*lineNo, "bloc action incomplet (add manquant)");
        return false;
    }
    r.addCount = parseLine(next, "add", r.add, MAX_ADD, *lineNo);
    if (r.addCount < 0) return false;
 
    if (!verifLine(file, next, lineNo)){
        parseError(*lineNo, "bloc action incomplet (delete manquant)");
        return false;
    }
    r.delCount = parseLine(next, "delete", r.del, MAX_DELETE, *lineNo);
    if (r.delCount < 0) return false;
 
    rules[ruleCount++] = r;
    return true;
}

bool loadProblem(const char* path){
    //Lit le fichier problème et remplit startState, finishState, rules et rulesCount

    FILE *file = fopen(path, "r");
    if (!file){
        printf("Erreur : impossible d'ouvrir le fichier\n");
        return false;
    }

    char line[MAX_LINE_LENGTH];
    int lineNo = 0;
    bool ok = true;
    ruleCount = 0;

    while (ok && verifLine(file, line, &lineNo)){
        if (strncmp(line, "start:", 6) == 0){
            ok = parseState(line, "start", &startState, lineNo);
        } else if (strncmp(line, "finish:", 7) == 0){
            ok = parseState(line, "finish", &finishState, lineNo);
        } else if (strncmp(line, "action:", 7) == 0){
            ok = parseAction(file, line, &lineNo);
        } else {
            parseError(lineNo, "ligne inconnue");
            ok = false;
        }
    }

    fclose(file);
    return ok;
}
