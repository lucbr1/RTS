# Bilan du code – RTS proies / prédateurs

> Ce qu'il reste à faire. Numéros de ligne = état actuel des fichiers.

---

## 1. Variables de l'UI pas encore branchées

| Variable | Où | Quoi faire |
|---|---|---|
| `meurtDeFaimH` / `tempsMortDeFaimH` | `Proie.cpp` | les proies ne mangent rien ➜ elles meurent toutes au bout de `tempsMortDeFaimH` jours. **Inutile sans végétation** (extension 4.4), sinon retirer de l'UI |
| `tauxMortaliteC` | — | jamais utilisée (TP 3.4) |
| `tauxMortaliteH` | — | jamais utilisée |
| `tauxReproductionH`, `tauxReproductionC` | — | utilisées par le code mais **absentes de l'UI** ➜ ajouter un curseur si besoin |

---

## 2. Ce qu'il reste du TP (partie 3)

### 3.1 (4) / 3.6 (6) – Export CSV ❗
- [ ] rien n'existe
- [ ] où : `RTSproject.cpp`, dans le `if (Space)` après `grid.gameLoop()`
- [ ] écrire `jour;nbProies;nbPredateurs` dans un `std::ofstream`
- [ ] vider / réécrire le fichier au `reset()`

### 3.1 – Proies seules (probabiliste)
- [x] chaque paire de proies a une probabilité `tauxReproductionH` de donner un faon (`Grille.cpp:374`)
- [ ] TP pur : reproduction avec une probabilité `p` **par proie**, sans rencontre ➜ option « probabiliste » vs « par rencontre » (actuel) si tu veux les deux

### 3.4 – Prédateurs seuls, mort aléatoire
- [ ] probabilité de mort `tauxMortaliteC` à chaque pas (dans `gameLoopPredateur`, `Grille.cpp:400`)
- [ ] actuellement seul le combat (3.5) est codé ; avec `seBatteC` sur off, les prédateurs seuls ne meurent plus du tout

### 3.6 (3) – Combat **ou** reproduction
- [ ] actuellement : combat des mâles **toujours** (si `seBatteC`), puis reproduction (`combatEtReproductionPredateurs`, `Grille.cpp:423`)
- [ ] TP : tirage au sort ➜ (1) combat **ou** (2) reproduction

### 3.6 (4) – Reproduction selon la satiété
- [ ] ajouter une condition `getSatiete() >= seuil` dans `naissancePredateur` (`Grille.cpp:460`)

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

## 3. Divers
- `tauxReproductionH = 0.05` et `tauxReproductionC = 0.02` sont maintenant des probabilités **par couple et par jour** ➜ les naissances sont rares, ajuster les valeurs
- seuil de faim `tempsMortDeFaimC / 2` (`Grille.cpp:481`) ➜ éventuellement une variable dans `Variable.hpp`
- si `spawnH` et `spawnC` sont tous les deux sur off ➜ `gameLoop()` ne fait rien, mais le jour avance quand même
- populations max à 1000 dans l'UI pour 2304 cases (64 × 36) ➜ beaucoup d'entités par case au départ
- pas d'animation pour une proie morte de faim (`MortNaturelle` affiche « loup mort de faim »)

---

## 4. Ordre conseillé
1. Export CSV
2. Mort aléatoire des prédateurs (3.4)
3. Combat **ou** reproduction (3.6 (3)) et satiété (3.6 (4))
4. Curseurs pour les taux dans l'UI
