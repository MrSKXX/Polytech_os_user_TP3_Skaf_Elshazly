#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
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
        execvp(P[0], P);
        fprintf(stderr, "biceps: %s: commande introuvable\n", P[0]);
        exit(1);
    }
    waitpid(pid, &status, 0);
    return status;
}

static int appliquerRedirections(void)
{
    int i = 0;
    int fd;
    while (i < NMots) {
        if (strcmp(Mots[i], "<") == 0 || strcmp(Mots[i], ">") == 0 ||
            strcmp(Mots[i], ">>") == 0 || strcmp(Mots[i], "2>") == 0 ||
            strcmp(Mots[i], "2>>") == 0) {

            if (i + 1 >= NMots) {
                fprintf(stderr, "biceps: erreur de syntaxe pres de '%s'\n", Mots[i]);
                return -1;
            }

            if (strcmp(Mots[i], "<") == 0) {
                fd = open(Mots[i+1], O_RDONLY);
                if (fd < 0) { perror(Mots[i+1]); return -1; }
                dup2(fd, STDIN_FILENO);
                close(fd);
            } else if (strcmp(Mots[i], ">") == 0) {
                fd = open(Mots[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd < 0) { perror(Mots[i+1]); return -1; }
                dup2(fd, STDOUT_FILENO);
                close(fd);
            } else if (strcmp(Mots[i], ">>") == 0) {
                fd = open(Mots[i+1], O_WRONLY | O_CREAT | O_APPEND, 0644);
                if (fd < 0) { perror(Mots[i+1]); return -1; }
                dup2(fd, STDOUT_FILENO);
                close(fd);
            } else if (strcmp(Mots[i], "2>") == 0) {
                fd = open(Mots[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd < 0) { perror(Mots[i+1]); return -1; }
                dup2(fd, STDERR_FILENO);
                close(fd);
            } else if (strcmp(Mots[i], "2>>") == 0) {
                fd = open(Mots[i+1], O_WRONLY | O_CREAT | O_APPEND, 0644);
                if (fd < 0) { perror(Mots[i+1]); return -1; }
                dup2(fd, STDERR_FILENO);
                close(fd);
            }

            free(Mots[i]);
            free(Mots[i+1]);
            int j;
            for (j = i; j < NMots - 2; j++) {
                Mots[j] = Mots[j+2];
            }
            Mots[NMots - 2] = NULL;
            Mots[NMots - 1] = NULL;
            NMots -= 2;
        } else {
            i++;
        }
    }
    return 0;
}

int execPipeline(char *cmd_seq)
{
    char *cmds[64];
    int num_cmds = 0;
    char *ptr = cmd_seq;
    char *tok;
    int i, j;
    int pipes[64][2];
    pid_t pids[64];
    int status = 0;

    while ((tok = strsep(&ptr, "|")) != NULL) {
        if (*tok != '\0') {
            cmds[num_cmds++] = tok;
        }
    }

    if (num_cmds == 0) return 0;

    if (num_cmds == 1) {
        if (analyseCom(cmds[0]) > 0) {
            int saved_in = dup(STDIN_FILENO);
            int saved_out = dup(STDOUT_FILENO);
            int saved_err = dup(STDERR_FILENO);

            if (appliquerRedirections() == 0) {
                if (!execComInt(NMots, Mots)) {
                    execComExt(Mots);
                }
            }

            dup2(saved_in, STDIN_FILENO);
            dup2(saved_out, STDOUT_FILENO);
            dup2(saved_err, STDERR_FILENO);
            close(saved_in);
            close(saved_out);
            close(saved_err);
        }
        return 0;
    }

    for (i = 0; i < num_cmds - 1; i++) {
        if (pipe(pipes[i]) < 0) {
            perror("biceps: pipe");
            return -1;
        }
    }

    for (i = 0; i < num_cmds; i++) {
        pids[i] = fork();
        if (pids[i] < 0) {
            perror("biceps: fork");
            return -1;
        }

        if (pids[i] == 0) {
            if (i > 0) {
                dup2(pipes[i-1][0], STDIN_FILENO);
            }
            if (i < num_cmds - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }

            for (j = 0; j < num_cmds - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            if (analyseCom(cmds[i]) > 0) {
                if (appliquerRedirections() == 0) {
                    if (execComInt(NMots, Mots)) {
                        exit(0);
                    }
                    execvp(Mots[0], Mots);
                    fprintf(stderr, "biceps: %s: commande introuvable\n", Mots[0]);
                }
            }
            exit(1);
        }
    }

    for (i = 0; i < num_cmds - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    for (i = 0; i < num_cmds; i++) {
        waitpid(pids[i], &status, 0);
    }

    return status;
}