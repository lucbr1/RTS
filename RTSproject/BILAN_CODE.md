# Bilan du code – RTS proies / prédateurs

> Ce qu'il reste à faire. Numéros de ligne = état actuel des fichiers.
>
> Choix gardés : reproduction **par rencontre** (avec une probabilité par couple) et déplacement d'une case **diagonales comprises / sur place possible**.

---

## 1. Variable de l'UI sans effet utile

| Variable | Où | Quoi faire |
|---|---|---|
| `meurtDeFaimH` / `tempsMortDeFaimH` | `Proie.cpp` | les proies ne mangent rien ➜ elles meurent toutes au bout de `tempsMortDeFaimH` jours. **Inutile sans végétation** (extension 4.4), sinon retirer de l'UI |

---

## 2. Ce qu'il reste du TP (partie 3)

### 3.1 (4) / 3.6 (6) – Export CSV ❗
- [ ] rien n'existe
- [ ] où : `RTSproject.cpp`, dans le `if (Space)` après `grid.gameLoop()`
- [ ] écrire `jour;nbProies;nbPredateurs` dans un `std::ofstream`
- [ ] vider / réécrire le fichier au `reset()`

### 3.6 (3) – Combat **ou** reproduction
- [ ] actuellement : combat des mâles **toujours** (si `seBatteC`), puis reproduction (`combatEtReproductionPredateurs`, `Grille.cpp:452`)
- [ ] TP : tirage au sort ➜ (1) combat **ou** (2) reproduction

### 3.6 (4) – Reproduction selon la satiété
- [ ] ajouter une condition `getSatiete() >= seuil` dans `naissancePredateur` (`Grille.cpp:487`)

### Extensions (partie 4)
- [ ] courbes de population dans l'interface
- [ ] diagramme de phases
- [ ] espérance de vie (mort selon l'âge, au lieu d'un taux fixe)
- [ ] végétation (donnerait un sens à `meurtDeFaimH`)

---

## 3. Divers
- seuil de faim `tempsMortDeFaimC / 2` (`Grille.cpp:510`) ➜ éventuellement une variable dans `Variable.hpp`
- si `spawnH` et `spawnC` sont tous les deux sur off ➜ `gameLoop()` ne fait rien, mais le jour avance quand même
---

## 4. Ordre conseillé
1. Export CSV
2. Combat **ou** reproduction (3.6 (3)) et satiété (3.6 (4))
3. Extensions
