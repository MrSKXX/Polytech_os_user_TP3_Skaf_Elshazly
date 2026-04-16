NOM: SKAF, EL SHAZLY
PRENOM: Georges, Ahmed

# BICEPS - Bel Interpréteur de Commandes des Elèves de Polytech Sorbonne

[cite_start]Ce projet consiste en la réalisation d'un interpréteur de commandes (shell) complet intégrant un protocole de communication réseau décentralisé (BEUIP) et un système d'échange de fichiers[cite: 4, 155, 288].

## Fonctionnalités implémentées

### Interpréteur de commandes (Shell)
* [cite_start]**Commandes Internes** : `exit`, `cd`, `pwd`, `vers` et `beuip`[cite: 201, 231, 234, 377].
* [cite_start]**Commandes Externes** : Exécution de n'importe quel binaire système via `fork` et `execvp`[cite: 217, 218].
* [cite_start]**Pipelines** : Support de l'enchaînement de commandes via le caractère `|`[cite: 269].
* [cite_start]**Redirections** : Gestion des redirections d'entrée/sortie (`<`, `>`, `>>`) et des erreurs (`2>`, `2>>`)[cite: 270].
* [cite_start]**Historique** : Mémorisation et persistance des commandes saisies via la librairie `readline`[cite: 158, 241].

### Protocole BEUIP (UDP)
* [cite_start]**Auto-détection** : Identification automatique des interfaces réseau et récupération des adresses de broadcast via `getifaddrs`[cite: 58, 59].
* [cite_start]**Gestion des présences** : Utilisation de messages UDP (codes '1' pour l'identification, '2' pour l'AR, '0' pour le départ) pour maintenir une liste chaînée dynamique des utilisateurs connectés[cite: 311, 313, 317, 367].
* [cite_start]**Multi-threading** : Le serveur UDP tourne dans un thread séparé avec gestion des accès concurrents via des mutex[cite: 25, 28, 47].

### Échange de fichiers (TCP - Bonus Partie 3)
* [cite_start]**Serveur TCP** : Un thread serveur dédié écoute sur le port 9998 pour gérer les requêtes de fichiers[cite: 93, 94].
* [cite_start]**Exploration** : Commande `beuip ls` pour lister le contenu du répertoire public d'un utilisateur distant[cite: 97, 101].
* [cite_start]**Transfert** : Commande `beuip get` pour télécharger un fichier depuis le répertoire `reppub/` d'un pair[cite: 110, 118].

## Structure du code

* [cite_start]**`biceps.c`** : Point d'entrée du programme, gestion de la boucle interactive et des commandes internes[cite: 155, 163, 222].
* [cite_start]**`gescom.c` / `gescom.h`** : Librairie de gestion des commandes (analyse syntaxique, exécution, pipes et redirections)[cite: 249, 250].
* [cite_start]**`creme.c` / `creme.h`** : Librairie réseau (Commandes Rapides pour l'Envoi de Messages Evolués) gérant les serveurs UDP/TCP et la liste des utilisateurs[cite: 288, 373, 374].

## Compilation et Utilisation

### Compilation
* **Standard** : `make` produit le binaire `biceps`.
* **Nettoyage** : `make clean` supprime les objets et les exécutables.
* **Valgrind** : `make memory-leak` produit le binaire `biceps-memory-leaks` avec les options `-g -O0` pour l'analyse mémoire.

### Commandes BEUIP
* `beuip start <pseudo>` : Lance les serveurs UDP et TCP.
* `beuip stop` : Arrête proprement les serveurs et informe le réseau.
* `beuip list` : Affiche les utilisateurs présents au format `IP : pseudo`.
* `beuip message <user> <message>` : Envoie un message privé à un utilisateur.
* `beuip message all <message>` : Envoie un message à tous les utilisateurs (broadcast).
* `beuip ls <pseudo>` : Liste les fichiers du répertoire `reppub` de l'utilisateur.
* `beuip get <pseudo> <nomfic>` : Télécharge le fichier spécifié dans le répertoire local `reppub/`.

## Tests de fuites mémoire
Pour vérifier l'intégrité de la mémoire, utilisez la commande suivante :
`valgrind --leak-check=full --track-origins=yes --errors-for-leak-kinds=all --error-exitcode=1 ./biceps-memory-leaks`