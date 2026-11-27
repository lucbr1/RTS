# Bilan du code – RTS proies / prédateurs

> Ce qu'il reste à faire. Numéros de ligne = état actuel des fichiers.
>
> Choix gardés : reproduction **par rencontre** (avec une probabilité par couple) et déplacement d'une case **diagonales comprises / sur place possible**.

---

## 1. Ce qu'il reste du TP (partie 3)

### 3.6 (3) – Combat **ou** reproduction
- [ ] actuellement : combat des mâles (probabilité `probaCombat`, si `seBatteC`) **puis** reproduction (`combatEtReproductionPredateurs`, `Grille.cpp:485`)
- [ ] TP : tirage au sort ➜ (1) combat **ou** (2) reproduction

### Extensions (partie 4)
- [x] courbes de population, diagramme de phases et export CSV
- [x] mort de vieillesse (loi de Gompertz, `isDead` dans `fonctions.cpp`)
- [x] végétation (herbe)

---

## 2. Diagramme de phases en « escargot » (spirale) – à retester
L'ancien test (~1000 simulations, aucune spirale, au mieux un tour) date d'**avant** l'herbe, la reproduction selon la satiété et la mort de Gompertz ➜ il n'est plus valable.
- avec les valeurs par défaut actuelles, les proies explosent (~19 000 au jour 320) : « Mort de faim » des proies est sur off, donc l'herbe limite leurs naissances mais ne les fait jamais mourir
- causes observées à l'époque, à vérifier :
  - reproduction **par rencontre** ➜ une population devenue petite ne retrouve plus de partenaire et s'éteint au lieu de remonter
  - tous les loups naissent avec la même satiété ➜ ils ont faim en même temps et meurent par vagues
- la reproduction selon la satiété (maintenant codée) était justement le lien manquant « plus de proies ➜ plus de louveteaux »

---

## 3. Divers
- seuil de faim des prédateurs `tempsMortDeFaimC / 2` (`Grille.cpp:523`) ➜ éventuellement une variable dans `Variable.hpp`
- « Repro. si satiété > » peut dépasser « Mort de faim après » (satiété max) ➜ plus aucune reproduction ; limiter le curseur au max de satiété
- si `spawnH` et `spawnC` sont tous les deux sur off ➜ `gameLoop()` ne fait rien, mais le jour avance quand même

---

## 4. Ordre conseillé
1. Combat **ou** reproduction (3.6 (3))
2. Retester les réglages pour l'escargot
