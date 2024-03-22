# Projet IN204 - Messagerie Cryptee 

#### Ethan Abimelech - Roy Alsaddi

## Introduction

Dans le cadre du projet IN204, noys avons réalisé une messagerie chiffrée. Ce projet contient entre-autres une partie serveur et une partie Client. N'ayant pas set-up le serveur sur un serveur dédié, pour tester la messagerie, il faudra lancer le serveur et les 2 clients sur le même ordinateur.

## Étapes à suivre pour lancer le test

1. Lancer le ``Serveur`` : cela nous permet d'etablir une connexion avec la base de donnees specifiee lors de l'initialisation. Pour cela, aller dans le dossier ''Serveur''.
2. Lancer le ``Client`` : une fenetre du premier utilisateur apparait.
3. Lancer une autre instance de ``Client`` a travers le terminal avec la commande "./client": une seconde fenetre du second utilisateur apparait.



## Example de communication entre les deux fenetres crées
 1. Ajouter le nom du second utilisateur dans l'interface du premier avec ``Add contact``

 2. Le choisir dans la meme interface parmi la liste des contacts (son nom apparait en tete de la discussion).

 3. Envoyer un message avec ``Send`

 4. Reception automatique du message dans la fenetre du second utilisateur.

PS: les noms des utilisateurs sont auto-generés et sont de la forme Client-i pour le ieme client. Ceci est du au fait qu'on test que sur un PC donc on s'authentifie la premiere fois avec un PIN et apres cette instance de client le reste est auto-genere.
