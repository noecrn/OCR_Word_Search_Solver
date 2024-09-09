# OCR Word Search Solver

## Description
Ce projet consiste à développer une application capable de résoudre une grille de mots cachés à partir d'une image. L'application utilise la reconnaissance optique de caractères (OCR) combinée à un réseau de neurones pour identifier les lettres dans une grille, puis résoudre la grille en trouvant les mots de la liste.

## Fonctionnalités principales
- **Chargement d'une image** : L'application prend une image d'une grille de mots cachés comme entrée.
- **Prétraitement** : Conversion de l'image en niveaux de gris et élimination des bruits visuels (grain, tâches, etc.).
- **Détection de la grille** : Identification des cases de la grille et de la liste de mots.
- **Reconnaissance des caractères** : Utilisation d'un réseau de neurones pour reconnaître les lettres dans la grille et la liste de mots.
- **Résolution de la grille** : Recherche et identification des mots cachés dans la grille.
- **Affichage des résultats** : Visualisation de la grille avec les mots trouvés entourés.
- **Sauvegarde** : Enregistrement de la grille résolue au format image.

## Prérequis
- Le projet est codé en **C**.
- Un compilateur C compatible avec les options `-Wall` et `-Wextra`.
- Utiliser l'environnement de développement fourni par l'école (machines de l'EPITA).

## Installation
1. Cloner le dépôt Git :
   ```
   git clone git@github.com:noecrn/OCR_Word_Search_Solver.git
   cd OCR_Word_Search_Solver
2. Compiler le projet avec le Makefile :
   ```
   make
## Utilisation
### Résolution d'une grille en ligne de commande
L'application dispose d'un programme de ligne de commande `solver` qui permet de résoudre une grille de mots cachés :
```
./solver <fichier_grille> <mot_a_trouver>
```
- `<fichier_grille>` : Un fichier texte contenant une grille de caractères.
- `<mot_a_trouver>` : Le mot à chercher dans la grille.

Exemple :
```
$ ./solver grid.txt hello
(9,8)(5,8)
```

### Interface graphique
L'interface graphique permet de charger une image, de la prétraiter et de résoudre la grille de mots cachés. Elle offre également la possibilité d'enregistrer la grille résolue en tant qu'image.

## Format des fichiers
Le fichier de la grille (`<fichier_grille>`) doit être un fichier texte contenant des caractères majuscules avec un minimum de 5 lignes et 5 colonnes.

Exemple :
```
HORIZONTAL
DXRAHCLBGA
DIKCILEOKC
IGAJHYLYHI
HGFGODTIOT
GDLROWKBFR
PLNRDNERGE
```

## Structure du dépôt
- `rapport_1.pdf` : Rapport de la première soutenance.
- `rapport_2.pdf` : Rapport de la soutenance finale.
- `Makefile` : Fichier de compilation.
- `solver` : Programme de résolution de grilles de mots cachés en ligne de commande.
- `README.md` : Ce fichier.
- `AUTHORS` : Fichier listant les membres de l'équipe.

## Auteurs
- Noé Cornu
- Célian Muller
- Aurélien Meyer
- Antoine Ramstein
