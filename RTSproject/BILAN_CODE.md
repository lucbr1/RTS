# Bilan du code – RTS proies / prédateurs

> Ce qu'il reste à faire. Numéros de ligne = état actuel des fichiers.

---

## 1. Bugs

### Prédateurs qui ne mangent plus quand « Meurent de faim » est sur off
- `CPredateur::seDeplacer` (`Predateur.cpp:15-23`) ne décrémente `satiete` que si `meurtDeFaimC`
- ➜ sur off, la satiété reste au max, `getSatiete() < tempsMortDeFaimC / 2` (`Grille.cpp:437`) n'est jamais vrai ➜ les loups ne mangent jamais
- **Fix** : toujours décrémenter `satiete` (sans descendre sous 0), ne mettre `mort = true` que si `meurtDeFaimC`

### Fuites mémoire (aucun `delete`)
Les entités sont créées avec `new` et jamais libérées :
- `reset()` ➜ `clear()` des cellules (`Grille.cpp:38-39`)
- morts de faim ➜ `retirerMorts()` (`Grille.cpp:321`)
- perdants du combat ➜ `Grille.cpp:389` et `:402`
- proies mangées ➜ `Grille.cpp:441`
- **Fix** : `delete` avant de retirer, ou passer en `std::unique_ptr`

---

## 2. Variables de l'UI pas encore branchées

| Variable | Où | Quoi faire |
|---|---|---|
| `seBatteC` | `combatEtReproductionPredateurs` (`Grille.cpp:395`) et `gameLoopPredateur` (`:376`) | si `false` ➜ garder **tous** les mâles |
| `meurtDeFaimH` / `tempsMortDeFaimH` | `Proie.cpp` | les proies ne mangent rien ➜ elles meurent toutes au bout de `tempsMortDeFaimH` jours. **Inutile sans végétation** (extension 4.4), sinon retirer de l'UI |
| `tauxReproductionH` | — | jamais utilisée (TP 3.1) |
| `tauxMortaliteC` | — | jamais utilisée (TP 3.4) |
| `tauxReproductionC`, `tauxMortaliteH` | — | jamais utilisées |

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
- [ ] actuellement : combat des mâles **toujours**, puis reproduction (`combatEtReproductionPredateurs`)
- [ ] TP : tirage au sort ➜ (1) combat **ou** (2) reproduction

### 3.6 (4) – Reproduction selon la satiété
- [ ] ajouter une condition `getSatiete() >= seuil` avant la naissance (`Grille.cpp:416`)

### 3.1 (3) – Déplacement
- `dx` et `dy` ∈ {-1, 0, 1} (`Entite.cpp:34-35`)
- ➜ diagonales possibles + entité qui reste sur place
- TP : **1 case** haut / bas / gauche / droite ➜ à toi de voir si tu gardes

### Extensions (partie 4)
- [ ] courbes de population dans l'interface
- [ ] diagramme de phases
- [ ] espérance de vie (mort selon l'âge)
- [ ] végétation (donnerait un sens à `meurtDeFaimH`)

---

## 4. Divers
- seuil de faim `tempsMortDeFaimC / 2` (`Grille.cpp:437`) ➜ éventuellement une variable dans `Variable.hpp`
- si `spawnH` et `spawnC` sont tous les deux sur off ➜ `gameLoop()` ne fait rien, mais le jour avance quand même
- populations max à 1000 dans l'UI pour 2304 cases (64 × 36) ➜ beaucoup d'entités par case au départ
- pas d'animation pour une proie morte de faim (`MortNaturelle` affiche « loup mort de faim »)

---

## 5. Ordre conseillé
1. Satiété des prédateurs quand `meurtDeFaimC` est off
2. Brancher `seBatteC`
3. Export CSV
4. `delete` / fuites mémoire
5. Points restants du TP (3.1 proba, 3.4, 3.6 (3)(4))
