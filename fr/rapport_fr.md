# Sudoku — CLI & GUI (C / SDL2)
**Binôme :** ODE_BRINO_Rafael_RIZO_Maria_Eduarda  
**Cours :** Projet C 2025–2026  
**Date :** 2025-10-17

## 1. Objectif
Réaliser une application Sudoku complète en C : (a) CLI texte ; (b) GUI SDL2 ; (c) solveur avec trace ; (d) générateur garantissant l’unicité ; (e) sauvegarde/chargement ; (f) indice.

## 2. À tester d’abord (TL;DR)
- Build : `make cli` et `make gui`.
- Démo rapide :

  ```bash
  ./build/sudoku_cli --generate 40 > k40.txt
  ./build/sudoku_cli --solve k40.txt
  ```
- GUI : `./build/sudoku_gui` → nouvelle partie, saisie clavier, **H** (indice), **S** (résoudre), **K** (sauver), **L** (charger), **B** (trace/pas-à-pas).
- Plan de validation : voir *INTEGRATION_VALIDATION.md* (EN & FR).

## 3. Périmètre fonctionnel
- CLI : générer k trous, charger/résoudre/afficher, lire fichier/stdin, écrire stdout.

- GUI (SDL2) : édition de la grille, saisie clavier, indice, résolution pas-à-pas avec trace, sauvegarde/chargement JSON.

- Générateur : enlève des cases jusqu’à k ; unicité vérifiée par DLX.

- Solveurs : DLX (Algorithm X), Backtracking (MRV + bitmasks), Propagation de contraintes (singles).

## 4. Choix de conception
- Découpage clair : *utils*, *solvers*, *generator*, *interface (cli/gui)*.

- API `Grid` unique pour I/O, validations, mutations.

- Graine RNG déterministe optionnelle pour tests.

- Trace découplée (événements) et consommée par la GUI.

## 5. Choix algorithmiques (pourquoi)
- **DLX** : très rapide et idéal pour tester l’unicité ; oracle de référence.

- **Backtracking + MRV/bitmask** : base simple ; excellent pour visualiser la recherche.

- **Propagation (singles)** : élagage économique avant la recherche.

- Détails : *JUSTIFICATION_ALGORITHMIQUE.md*.

## 6. Intégration & validation (comment)
- Recettes mono-commande pour l’enseignant.

- Jeux de données : générer k∈{30,40,50} et vérifier (temps, unicité, correction).

- Sauvegarde/chargement ; tests GUI (clavier/souris).

- Liste complète : *INTEGRATION_VALIDATION.md*.

## 7. Limites connues
- Difficulté≈k (nombre de cases vides), pas un score “humain”.

- GUI minimaliste ; pas de localisation runtime.

- Règles humaines limitées aux singles.

## 8. Structure (attendue)
```
include/            # en-têtes
src/utils/          # grid, io, rand, trace
src/solvers/        # dlx, backtracking, mrv_bitmask, constraint_propagation, rules
src/generator/      # generator
src/interface/      # cli, formatter, save, gui, gui_draw, gui_events, gui_solver
build/              # artefacts
data/               # grilles, save.json
```
