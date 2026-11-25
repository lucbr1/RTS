# Bilan du code – RTS proies / prédateurs

> Numéros de ligne = état actuel des fichiers.

---

## 1. Bugs à corriger en priorité

### Proies qui ne se reproduisent plus en mode couplé
- `Reproduction()` n'est appelée que dans `gameLoopProie()` (`Grille.cpp:325`)
- `gameLoopProiePredateur()` ne l'appelle jamais
- ➜ en mode proies + prédateurs, les proies ne font que diminuer
- **Fix** : appeler `Reproduction();` **après** la boucle des cellules (fin de `gameLoopProiePredateur`, ~ligne 522)
  - après la prédation ➜ une proie mangée ne peut plus se reproduire (TP 3.6)
  - `nbProies++` dans `Reproduction()` reste cohérent avec le recomptage

### UI ≠ comportement réel
- `meurtDeFaimC = false` par défaut, mais les prédateurs **meurent quand même de faim** (`Predateur.cpp:41-45`, `deplacementPredateurs(true)` ligne 414)
- ➜ l'interrupteur affiche « off » alors que c'est actif

### Fuites mémoire (aucun `delete`)
Toutes les entités sont créées avec `new` et jamais libérées :
- `reset()` ➜ `cellule.vider()` (`Grille.cpp:57`)
- prédateurs morts de faim (`Grille.cpp:429-432`)
- mâles perdants du combat (`Grille.cpp:449-495`)
- proies mangées (`Grille.cpp:511`)
- `gameLoopPredateur()` (`Grille.cpp:401-403`)
- **Fix** : `delete` avant de retirer, ou passer en `std::unique_ptr`

---

## 2. Variables de l'UI pas encore branchées

| Variable | Où l'ajouter | Quoi faire |
|---|---|---|
| `meurtDeFaimC` | `Predateur.cpp:40-49` | toujours décrémenter `satiete`, ne mettre `mort = true` que si `meurtDeFaimC` |
| `tempsMortDeFaimC` | `Predateur.hpp:9`, `Grille.cpp:512` | remplacer le `10` en dur par `tempsMortDeFaimC` |
| ↳ seuil de faim | `Grille.cpp:505` | remplacer `< 5` par `< tempsMortDeFaimC / 2` |
| `estCeQueTempsAvantReproductionH` | `Grille.cpp:353-354` | `if (...)` avant `setTempsAvantReproduction` |
| `estCeQueTempsAvantReproductionC` | `Grille.cpp:477-478` | idem |
| `seBatteC` | `Grille.cpp:451-453` et `:483-487` | si `false` ➜ garder **tous** les mâles |
| ↳ mode prédateurs seuls | `Grille.cpp:396-403` | idem |
| `meurtDeFaimH` / `tempsMortDeFaimH` | — | les proies ne mangent rien ➜ **inutile sans végétation** (extension 4.4) ; sinon retirer de l'UI |
| `tauxReproductionH` | — | jamais utilisée (voir TP 3.1 ci-dessous) |
| `tauxMortaliteC` | — | jamais utilisée (voir TP 3.4 ci-dessous) |
| `tauxReproductionC`, `tauxMortaliteH` | — | jamais utilisées |

⚠ Pour `meurtDeFaimC` : **ne pas** juste passer `meurtDeFaimC` à `deplacementPredateurs(...)`
- sinon la satiété ne baisse plus ➜ les prédateurs n'ont jamais faim ➜ ne mangent plus

---

## 3. Ce qu'il reste du TP (partie 3)

### 3.1 – Proies seules (probabiliste)
- [ ] reproduction avec une probabilité `p` (`tauxReproductionH`) à chaque pas
- [ ] il faut un tirage `double` entre 0 et 1 ➜ ajouter `double randomReel()` dans `fonctions.cpp`
- [ ] option : mode « probabiliste » vs « par rencontre » (actuel)

### 3.1 (4) / 3.6 (6) – Export CSV ❗
- [ ] rien n'existe
- [ ] où : `RTSproject.cpp`, dans le `if (Space)` après `grid.gameLoop()`
- [ ] écrire `jour;nbProies;nbPredateurs` dans un `std::ofstream`
- [ ] vider / réécrire le fichier au `reset()`

### 3.4 – Prédateurs seuls, mort aléatoire
- [ ] probabilité de mort `tauxMortaliteC` à chaque pas (dans `gameLoopPredateur`)
- [ ] actuellement seul le combat (3.5) est codé

### 3.6 (3) – Combat **ou** reproduction
- [ ] actuellement : combat des mâles **toujours**, puis reproduction
- [ ] TP : tirage au sort ➜ (1) combat **ou** (2) reproduction

### 3.6 (4) – Reproduction selon la satiété
- [ ] ajouter une condition `getSatiete() >= seuil` (`Grille.cpp:468-469`)

### 3.6 (5) – Mort de faim
- [x] fait (`Predateur.cpp:41`), à brancher sur `meurtDeFaimC` (voir §2)

### 3.1 (3) – Déplacement
- `dx` et `dy` ∈ {-1, 0, 1} (`Entite.cpp:28-29`, `Predateur.cpp:54-55`)
- ➜ diagonales possibles + entité qui reste sur place
- TP : **1 case** haut / bas / gauche / droite
- à toi de voir si tu gardes ou non

### Extensions (partie 4)
- [ ] courbes de population dans l'interface
- [ ] diagramme de phases
- [ ] espérance de vie (mort selon l'âge)
- [ ] végétation (donnerait un sens à `meurtDeFaimH`)

---

## 4. Ce qui ne convient plus / à nettoyer

### Grille
- `grille` a **65 × 37** cellules, l'écran n'en montre que **64 × 36** (`Grille.cpp:13-14`, `+ 1`)
  - la dernière colonne / ligne n'est jamais utilisée (déplacements limités à `maxX = 63`)
  - ➜ utiliser `windowWidth / cellSize` sans `+1` pour `grille.resize`
- `reset()` : `randomizer(0, size - 2)` ➜ ok seulement à cause du `+1` ci-dessus
- `reset()` : `nbProies = 0; nbPredateurs = 0;` dans la double boucle ➜ à sortir de la boucle

### Valeurs en dur ➜ mettre dans `Variable.hpp`
- âge min de reproduction `30` (`Grille.cpp:341`, `:468-469`)
- satiété max `10` (`Predateur.hpp:9`, `Grille.cpp:512`)
- seuil de faim `5` (`Grille.cpp:505`)

### Code mort / inutile
- `numLinesX` / `numLinesY` jamais utilisés (`Proie.cpp:5-6`, `Predateur.cpp:6-7`)
- `CEntite` calcule une position aléatoire (`Entite.cpp:9`) aussitôt écrasée par les constructeurs enfants
- `CEntite::afficher` (rond vert) jamais utilisé
- paramètre `useSatiete` ignoré pour les proies (`deplacementProies`)
- `Grille.cpp:437` : `predateurAffiche = ...` puis `viderpredateurs()` ligne 449 ➜ ligne inutile
- `#include <map>` inutile (`RTSproject.cpp`)

### Lisibilité
- `Grille.cpp:459` et `:462` : deux variables `femelle` (la 2ᵉ masque la 1ʳᵉ) ➜ renommer
- `CPredateur::seDeplacer` recopie tout `CEntite::seDeplacer` ➜ appeler `CEntite::seDeplacer()` après la gestion de la satiété
- `Reproduction()` ➜ renommer `reproductionProies()` (ne concerne que les proies)
- paramètre `annee` dans `CEntite(unsigned int annee)` ➜ ce sont des **jours**
- `satiete <= 0` sur un `unsigned` (`Predateur.cpp:41`) ➜ revient à `== 0`

### Divers
- si `spawnH` et `spawnC` sont tous les deux sur off ➜ `gameLoop()` ne fait rien, mais le jour avance quand même
- populations max à 1000 dans l'UI pour ~2300 cases ➜ beaucoup d'entités par case au départ

---

## 5. Ordre conseillé
1. Âge initial (`age = 0`)
2. `Reproduction()` en mode couplé
3. Brancher `meurtDeFaimC` + `tempsMortDeFaimC`
4. Brancher `seBatteC` et les délais de reproduction
5. Export CSV
6. `delete` / fuites mémoire
7. Points restants du TP (3.1 proba, 3.4, 3.6 (3)(4))
8. Nettoyage
