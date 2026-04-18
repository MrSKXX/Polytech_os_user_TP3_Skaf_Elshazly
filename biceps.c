#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "gescom.h"
#include "creme.h"

static int should_exit = 0;

int Sortie(int N, char **P)
{
    (void)N;
    (void)P;
    should_exit = 1;
    return 1;
}

int CommandeCD(int N, char **P)
{
    if (N > 1) {
        if (chdir(P[1]) != 0) perror("biceps: cd");
    } else {
        char *home = getenv("HOME");
        if (home != NULL) {
            if (chdir(home) != 0) perror("biceps: cd");
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
    printf("biceps version 3.0\n");
    return 1;
}

int CommandeBeuip(int N, char **P)
{
    if (N < 2) {
        fprintf(stderr, "Utilisation: beuip start|stop|list|message|ls|get ...\n");
        return 1;
    }

    if (strcmp(P[1], "start") == 0 && N >= 3) {
        beuip_start(P[2]);
    } else if (strcmp(P[1], "stop") == 0) {
        beuip_stop();
    } else if (strcmp(P[1], "list") == 0) {
        beuip_liste();
    } else if (strcmp(P[1], "ls") == 0 && N >= 3) {
        beuip_ls(P[2]);
    } else if (strcmp(P[1], "get") == 0 && N >= 4) {
        beuip_get(P[2], P[3]);
    } else if (strcmp(P[1], "message") == 0 && N >= 4) {
        char msg[1024] = "";
        int i;
        size_t space_left = sizeof(msg) - 1;

        for (i = 3; i < N; i++) {
            size_t wlen = strlen(P[i]);
            if (wlen >= space_left) break;
            strcat(msg, P[i]);
            space_left -= wlen;
            if (i < N - 1 && space_left > 1) {
                strcat(msg, " ");
                space_left--;
            }
        }

        if (strcmp(P[2], "all") == 0) {
            beuip_mess_all(msg);
        } else {
            beuip_mess_pseudo(P[2], msg);
        }
    } else {
        fprintf(stderr, "beuip: commande inconnue ou parametres manquants\n");
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
}

int main(void)
{
    char *line;
    char *prompt;
    char *user;
    char *home;
    char hostname[256];
    char promptchar;
    char hist_file[512];
    int have_hist = 0;

    signal(SIGINT, SIG_IGN);
    majComInt();

    home = getenv("HOME");
    if (home != NULL) {
        snprintf(hist_file, sizeof(hist_file), "%s/.biceps_history", home);
        read_history(hist_file);
        have_hist = 1;
    }

    prompt = malloc(512);
    if (prompt == NULL) {
        fprintf(stderr, "biceps: erreur malloc prompt\n");
        return 1;
    }

    user = getenv("USER");
    if (user == NULL) user = "unknown";

    if (gethostname(hostname, sizeof(hostname)) != 0) {
        strcpy(hostname, "unknown");
    }

    promptchar = (getuid() == 0) ? '#' : '$';
    snprintf(prompt, 512, "%s@%s%c ", user, hostname, promptchar);

    while (!should_exit) {
        line = readline(prompt);
        if (line == NULL) break;

        if (*line != '\0') add_history(line);

        char *line_ptr = line;
        char *cmd_seq;
        while ((cmd_seq = strsep(&line_ptr, ";")) != NULL) {
            execPipeline(cmd_seq);
            if (should_exit) break;
        }
        free(line);
    }

    if (have_hist) write_history(hist_file);

    beuip_stop();
    cleanupMots();
    free(prompt);
    clear_history();

    return 0;
}