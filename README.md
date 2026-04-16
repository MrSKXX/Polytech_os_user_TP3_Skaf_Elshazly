NOM: SKAF, EL SHAZLY
PRENOM: Georges, Ahmed

# BICEPS - Bel Interpréteur de Commandes des Elèves de Polytech Sorbonne

## Ce qui a été fait
Ce projet rassemble l'ensemble des TP du module OS User. Il implémente un interpréteur de commandes interactif (shell) couplé à un client/serveur de messagerie décentralisé utilisant le protocole BEUIP (BEUI over IP) via UDP.

Les fonctionnalités implémentées comprennent :
- Un shell interactif avec historique (readline), exécution de commandes internes et externes.
- La gestion des pipelines (`|`) et des redirections (`>`, `>>`, `<`, `2>`, `2>>`).
- Un serveur UDP multi-threadé permettant l'adaptation automatique aux réseaux (récupération de l'adresse de broadcast via `getifaddrs`).
- La gestion sécurisée et concurrente (via `mutex`) d'une liste chaînée d'utilisateurs connectés.
- L'absence totale de fuites mémoire (vérifié avec Valgrind).

## Structure du code

Le code est divisé en modules pour assurer sa lisibilité et sa maintenabilité :

- **`biceps.c`** : Fichier principal contenant la boucle d'interaction avec l'utilisateur (prompt), la gestion de l'historique et les définitions des commandes internes (`cd`, `pwd`, `exit`, `vers`, `beuip`).
- **`gescom.c` / `gescom.h`** : Librairie gérant l'analyse des lignes de commande (`analyseCom`), la structuration des processus, les redirections, les pipes, et l'exécution des commandes via `execvp`.
- **`creme.c` / `creme.h`** : Librairie réseau implémentant le protocole BEUIP. Elle contient le thread du serveur UDP (pour écouter les messages en arrière-plan), la gestion de la liste chaînée des utilisateurs (avec `pthread_mutex`), et les fonctions d'envoi de messages (`beuip_start`, `beuip_mess_all`, etc.). L'adresse de broadcast est définie par une macro unique.
- **`Makefile`** : Gère la compilation stricte (`-Wall -Werror`), produit l'exécutable `biceps`, et fournit les cibles `memory-leak` (compilation pour Valgrind) et `clean`.