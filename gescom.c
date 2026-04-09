#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "gescom.h"

#define NBMAXMOTS 64
#define NBMAXC    10

char **Mots = NULL;
int NMots = 0;

typedef struct {
    char *nom;
    int (*func)(int, char **);
} ComInt;

static ComInt tabComInt[NBMAXC];
static int nbComInt = 0;

int analyseCom(char *b)
{
    char *tmp;
    char *token;
    char *ptr;
    int i;

    if (Mots == NULL) {
        Mots = malloc(NBMAXMOTS * sizeof(char *));
        if (Mots == NULL) {
            fprintf(stderr, "biceps: erreur malloc Mots\n");
            exit(1);
        }
    }

    for (i = 0; i < NMots; i++) {
        free(Mots[i]);
        Mots[i] = NULL;
    }
    NMots = 0;

    if (b == NULL || *b == '\0')
        return 0;

    tmp = strdup(b);
    ptr = tmp;

    while ((token = strsep(&ptr, " \t\n")) != NULL) {
        if (*token != '\0' && NMots < NBMAXMOTS - 1) {
            Mots[NMots] = strdup(token);
            NMots++;
        }
    }
    Mots[NMots] = NULL;

    free(tmp);
    return NMots;
}

void ajouteCom(char *nom, int (*func)(int, char **))
{
    if (nbComInt >= NBMAXC) {
        fprintf(stderr, "biceps: trop de commandes internes (max %d)\n", NBMAXC);
        exit(1);
    }
    tabComInt[nbComInt].nom  = nom;
    tabComInt[nbComInt].func = func;
    nbComInt++;
}

void listeComInt(void)
{
    int i;
    printf("commandes internes (%d) :\n", nbComInt);
    for (i = 0; i < nbComInt; i++)
        printf("  %s\n", tabComInt[i].nom);
}

int execComInt(int N, char **P)
{
    int i;
    for (i = 0; i < nbComInt; i++) {
        if (strcmp(P[0], tabComInt[i].nom) == 0) {
            tabComInt[i].func(N, P);
            return 1;
        }
    }
    return 0;
}

int execComExt(char **P)
{
    pid_t pid;
    int status;

    pid = fork();
    if (pid < 0) {
        perror("biceps: fork");
        return -1;
    }
    if (pid == 0) {
#ifdef TRACE
        fprintf(stderr, "TRACE: execvp(%s)\n", P[0]);
#endif
        execvp(P[0], P);
        fprintf(stderr, "biceps: %s: commande introuvable\n", P[0]);
        exit(1);
    }
    waitpid(pid, &status, 0);
    return status;
}