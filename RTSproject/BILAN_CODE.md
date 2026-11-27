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
- [ ] où : `RTSproject.cpp`, dans le `if (Space)`, au même endroit que `ui.enregistrer(...)` (les mêmes valeurs alimentent déjà les graphiques)
- [ ] écrire `jour;nbProies;nbPredateurs` dans un `std::ofstream`
- [ ] vider / réécrire le fichier au `reset()`

### 3.6 (3) – Combat **ou** reproduction
- [ ] actuellement : combat des mâles **toujours** (si `seBatteC`), puis reproduction (`combatEtReproductionPredateurs`, `Grille.cpp:454`)
- [ ] TP : tirage au sort ➜ (1) combat **ou** (2) reproduction

### 3.6 (4) – Reproduction selon la satiété
- [ ] ajouter une condition `getSatiete() >= seuil` dans `naissancePredateur` (`Grille.cpp:489`)

### Extensions (partie 4)
- [x] courbes de population (proies, prédateurs, les deux) et diagramme de phases (boutons « Graphiques » de la barre latérale)
- [ ] espérance de vie (mort selon l'âge, au lieu d'un taux fixe)
- [ ] végétation (donnerait un sens à `meurtDeFaimH`)

---

## 3. Diagramme de phases en « escargot » (spirale)
Testé sur ~1000 simulations avec tous les réglages du menu : **aucun réglage ne donne de spirale** avec le code actuel.
- meilleur réglage trouvé ➜ **un seul tour** (pic de proies puis pic de loups décalé), puis extinction des deux espèces vers le jour 550-1100 :
  proies 800 / 75 % reproduction / mortalité 1 % / délai 10 j — prédateurs 100 / 30 % reproduction / mort naturelle off / délai off / meurent de faim **on**, 40 j / ne se battent pas
- causes :
  - la reproduction des loups ne dépend pas de la nourriture ➜ beaucoup de proies ne donnent pas plus de louveteaux
  - reproduction **par rencontre** des deux espèces ➜ quand une population devient petite, les couples ne se croisent plus et elle s'éteint au lieu de remonter
  - tous les loups naissent avec la même satiété ➜ ils ont faim en même temps et meurent par vagues
- la condition de satiété (3.6 (4)) **seule ne suffit pas** (testée : toujours < 1 tour) ; il faudrait aussi rendre la reproduction des loups plus fréquente quand ils sont bien nourris

---

## 4. Divers
- seuil de faim `tempsMortDeFaimC / 2` (`Grille.cpp:512`) ➜ éventuellement une variable dans `Variable.hpp`
- si `spawnH` et `spawnC` sont tous les deux sur off ➜ `gameLoop()` ne fait rien, mais le jour avance quand même

---

## 5. Ordre conseillé
1. Export CSV
2. Combat **ou** reproduction (3.6 (3)) et satiété (3.6 (4))
3. Extensions
