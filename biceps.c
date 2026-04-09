#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "gescom.h"
#include "creme.h"

int Sortie(int N, char **P)
{
    (void)N;
    (void)P;
    printf("Fermeture de l'interpreteur.\n");
    beuip_stop();
    exit(0);
}

int CommandeCD(int N, char **P)
{
    if (N > 1) {
        if (chdir(P[1]) != 0) {
            perror("biceps: cd");
        }
    } else {
        char *home = getenv("HOME");
        if (home != NULL) {
            if (chdir(home) != 0) {
                perror("biceps: cd");
            }
        } else {
            fprintf(stderr, "biceps: cd: variable HOME non definie\n");
        }
    }
    return 1;
}

int CommandePWD(int N, char **P)
{
    char cwd[1024];
    (void)N; 
    (void)P;

    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        perror("biceps: pwd");
    }
    return 1;
}

int CommandeVers(int N, char **P)
{
    (void)N; 
    (void)P;
    
    printf("biceps version 2.0\n");
    printf("gescom version %s\n", GESCOM_VERSION);
    printf("creme version %s\n", CREME_VERSION);
    return 1;
}

int CommandeBeuip(int N, char **P)
{
    if (N < 2) {
        fprintf(stderr, "Utilisation: beuip start <pseudo> | beuip stop\n");
        return 1;
    }
    
    if (strcmp(P[1], "start") == 0) {
        if (N >= 3) {
            beuip_start(P[2]);
            printf("Serveur BEUIP demarre (pseudo: %s)\n", P[2]);
        } else {
            fprintf(stderr, "Utilisation: beuip start <pseudo>\n");
        }
    } else if (strcmp(P[1], "stop") == 0) {
        beuip_stop();
        printf("Serveur BEUIP arrete.\n");
    } else {
        fprintf(stderr, "Commande beuip non reconnue.\n");
    }
    return 1;
}

int CommandeMess(int N, char **P)
{
    if (N < 2) {
        fprintf(stderr, "Utilisation: mess liste | mess all <msg> | mess <pseudo> <msg>\n");
        return 1;
    }

    if (strcmp(P[1], "liste") == 0) {
        beuip_liste();
    } else if (strcmp(P[1], "all") == 0) {
        if (N >= 3) {
            beuip_mess_all(P[2]);
        } else {
            fprintf(stderr, "Utilisation: mess all <message>\n");
        }
    } else {
        if (N >= 3) {
            beuip_mess_pseudo(P[1], P[2]);
        } else {
            fprintf(stderr, "Utilisation: mess <pseudo> <message>\n");
        }
    }
    return 1;
}

void majComInt(void)
{
    ajouteCom("exit", Sortie);
    ajouteCom("cd", CommandeCD);
    ajouteCom("pwd", CommandePWD);
    ajouteCom("vers", CommandeVers);
    ajouteCom("beuip", CommandeBeuip);
    ajouteCom("mess", CommandeMess);
}

int main(void)
{
    char *line;
    char *prompt;
    char *user;
    char hostname[256];
    char promptchar;
    int i;
    char hist_file[512];
    char *home;

    signal(SIGINT, SIG_IGN);

    majComInt();
#ifdef TRACE
    listeComInt();
#endif

    home = getenv("HOME");
    if (home != NULL) {
        snprintf(hist_file, sizeof(hist_file), "%s/.biceps_history", home);
        read_history(hist_file);
    }

    prompt = malloc(512 * sizeof(char));
    if (prompt == NULL) {
        fprintf(stderr, "biceps: erreur malloc prompt\n");
        exit(1);
    }

    user = getenv("USER");
    if (user == NULL)
        user = "unknown";

    if (gethostname(hostname, sizeof(hostname)) != 0)
        strcpy(hostname, "unknown");

    if (getuid() == 0)
        promptchar = '#';
    else
        promptchar = '$';

    snprintf(prompt, 512, "%s@%s%c ", user, hostname, promptchar);

    while ((line = readline(prompt)) != NULL) {
        if (*line != '\0') {
            add_history(line);
        }

        char *line_ptr = line;
        char *cmd_seq;

        while ((cmd_seq = strsep(&line_ptr, ";")) != NULL) {
            if (analyseCom(cmd_seq) > 0) {
#ifdef TRACE
                fprintf(stderr, "TRACE: commande=%s, NMots=%d\n", Mots[0], NMots);
#endif
                if (!execComInt(NMots, Mots))
                    execComExt(Mots);
            }
        }
        free(line);
    }

    printf("\nSortie de biceps. Au revoir !\n");

    if (home != NULL) {
        write_history(hist_file);
    }

    free(prompt);
    
    if (Mots != NULL) {
        for (i = 0; i < NMots; i++) {
            free(Mots[i]);
        }
        free(Mots);
    }

    beuip_stop();
    return 0;
}