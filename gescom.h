#ifndef GESCOM_H
#define GESCOM_H

extern char **Mots;
extern int NMots;

int analyseCom(char *b);
void ajouteCom(char *nom, int (*func)(int, char **));
void listeComInt(void);
int execComInt(int N, char **P);
int execComExt(char **P);
int execPipeline(char *cmd_seq);
void cleanupMots(void);

#endif