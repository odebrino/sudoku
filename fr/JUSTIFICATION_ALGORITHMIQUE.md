# Justification Algorithmique (FR)
**Binôme :** ODE_BRINO_Rafael_RIZO_Maria_Eduarda — **Date :** 2025-10-17

## 1) Solveurs
### 1.1 DLX (Algorithm X, Dancing Links)
- Sudoku → exact cover (4 familles de contraintes : case, ligne, colonne, boîte).

- DLX : suppression/restauration O(1) ; très efficace ; pratique pour compter le nb de solutions.

**Avantages :** rapidité, oracle d’unicité.  
**Inconvénients :** moins pédagogique en pas-à-pas.

### 1.2 Backtracking + MRV + Bitmasks
- Variables=cases ; domaine=1–9 ; **MRV** choisit la case la plus contrainte ; bitmasks pour les candidats.

- Idéal pour visualiser l’arbre de recherche dans la GUI.

**Avantages :** simplicité, traçabilité.  
**Inconvénients :** exponentiel dans le pire cas ; dépend des heuristiques.

### 1.3 Propagation de contraintes (Singles)
- **Naked** et **Hidden singles** jusqu’au point fixe ; réduit le branchement.

## 2) Complexité (informelle)
- DLX : backtracking sur exact cover ; ms en pratique pour des grilles classiques.

- BT+MRV : exponentiel pire cas ; en pratique rapide avec singles+MRV.

- Propagation : coût négligeable par mise à jour.

## 3) Génération avec unicité
- À partir d’une solution complète → retirer des cases jusqu’à k.

- Après chaque retrait, DLX avec arrêt anticipé si ≥2 solutions ; sinon on annule le retrait.

- Randomisation : ordre des cases mélangé ; graine optionnelle.

## 4) Pourquoi plusieurs solveurs ?
- Pédagogie (BT trace) + Performance/validation (DLX) + UX (singles réduisent l’espace de recherche).

- Vérification croisée pour éviter les régressions.
