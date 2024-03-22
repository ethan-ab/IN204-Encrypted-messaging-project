# Projet IN204 - Messagerie Cryptee 

#### Roy AlSaddi - Ethan AbiMelech 

## Utilisation de CLion

On a decide d'utiliser CLion, une IDE developpee par JetBrains, afin de compiler et de tester notre application de messagerie cryptee. En effet, elle offre un support optimisé pour C/C++, des outils de débogage puissants, et une gestion intégrée de base de données, facilitant ainsi le développement sécurisé et efficace.

## Etapes a suivre pour lancer le test

1. Lancer le ``Serveur`` sur CLion: cela nous permet d'etablir une connexion avec la base de donnees specifiee lors de l'initialisation.
2. Lancer le ``Client`` sur CLion: une fenetre du premier utilisateur apparait.
<div align="center">
    <img src="photos test/image.png" width="300" height="200">
</div>

3. Lancer une autre instance de ``Client`` a travers le terminal avec la commande "./client": une seconde fenetre du second utilisateur apparait.
<div align="center">
    <img src="photos test/image-1.png" width="300" height="200">
</div>


## Example de communication entre les deux fenetres crees
 1. Ajouter le nom du second utilisateur dans l'interface du premier avec ``Add contact``
<div align="center">
    <img src="photos test/image-3.png" width="300" height="200">
</div>

 2. Le choisir dans la meme interface parmi la liste des contacts (son nom apparait en tete de la discussion).
<div align="center">
    <img src="photos test/image-4.png" width="300" height="200">
</div>

 3. Envoyer un message avec ``Send`
<div align="center">
    <img src="photos test/image-5.png" width="300" height="200">
</div>

 4. Reception automatique du message dans la fenetre du second utilisateur.
<div align="center">
    <img src="photos test/image-6.png" width="300" height="200">
</div>

PS: les noms des utilisateurs sont auto-generes et sont de la forme Client-i pour le ieme client. Ceci est du au fait qu'on test que sur un PC donc on s'authentifie la premiere fois avec un PIN et apres cette instance de client le reste est auto-genere.