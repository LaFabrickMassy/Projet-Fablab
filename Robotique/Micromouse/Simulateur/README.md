# Maze simulator

Le simulateur de labyrinthe comprend la génération de labyrinthes, la recherche du chemin optimal et la simulation du robot.

## Mode d'emploi

Lancer le script `maze_simu.py`. Par défaut, le labyrinthe est de taille 18x18. Le menu *Maze/Change size* permet de changer la taille du labyrinthe.

Le menu *Maze/New* ou la touche `n` lance la génération d'un nouveau labyrinthe.
Le menu *Maze/Solve* ou la touche `s` calcule le chemin optimal et l'affiche.

Les labyrinthes générés sont connexes : toutes les cellules sont accessibles, et maximaux : on ne peut pas ajouter de mur sans séparer au moins une cellule.

## A faire
- Sauvegarde/chargement de labyrinthes
- Modification d'un labyrinthe généré en ajoutant/supprimant des murs à la souris
- Mouvement du robot

<!------------------------------------------------------------------------->

## Algorithmique

### Labyrinthe

Un labyrinthe de taille n,m (n=nombre de colonnes, m=nombre de lignes) est défini par ses 
murs, horizontaux et verticaux. En comptant les murs des bords, on a
- (n+1)\*m murs verticaux,
- n\*(m+1) murs horizontaux.

On ne considère que des labyrinthes dont les bords sont murés (pour ne pas que le robot 
tombe du labyrinthe). Il donc est inutile de mémoriser les murs des bords. Pour simplifier 
les algorithmes, on ne mémorisera que les n\*m murs verticaux et les n\*m murs horizontaux, incluant les murs des bords de la première ligne et de la première colonne, mais en ignorant les murs de la dernière ligne et de la dernière colonne.  

Un labyrinthe est associé à un graphe. Les noeuds sont les cellules. Une arête relie deux 
noeuds si ces deux cellules sont adjacentes et si elles ne sont pas séparées par un mur.

La représentation par un graphe permet de
- Vérifier qu'il y a un chemin entre deux cellules en calculant les composantes connexes du graphe et en regardant si les noeuds associés à ces deux cellules sont dans la même composante connexe 
- Calculer le plus court chemin entre deux cellules par l'algorithme de Dijkstra

#### Génération d'un labyrinthe connexe maximal

### Graphe

#### Composantes connexes

#### Algorithme de Dijkstra

### Robot

L'état d'un robot est donné par
- sa position (x,y)
- son orientation h, angle par rapport à l'horizontale
- sa vitesse v

Le robot est circulaire de rayon r. La position du robot est le centre du cercle.

Le robot est équipé d'un certain nombre de capteurs de distance, dont l'axe de visée est 
décalé par rapport à l'axe du robot. 

#### Avance du robot

L'avance du robot se fait par pas de temps en commandant la vitesse du robot et la courbure 
de la trajectoire. Le rayon de courbure, inverse de la courbure, est le rayon de la trajectoire. 

Par convention, une courbure positive indique un virage vers la droite. Une courbure nulle correspond à un "tout droit", +\infty une rotation sur place vers la droite et -\infty une rotation sur place vers la gauche.

Par convension toujours, la vitesse considérée lors d'un virage est celle de la roue extérieure. Pour les courbures faibles, l'écart de vitesse entre les deux roues est négligeable mais pour les virages serrés, voire sur place, les deux roues peuvent avoient une grande différence de vitesse ou bien ne pas tourner dans le même sens.

#### Simulation des capteurs de distance


<!------------------------------------------------------------------------->
## Code

### Labyrinthe

le fichier `maze.py` contient la définition de la classe `Maze`.

### Graphe

Le fichier `graph.py` contient la définition de la classe `Graph`.

### Robot

Le fichier `robot.py` contient la définition de la classe `Robot`.
