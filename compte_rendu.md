<p align="right">NI Yuzhen, JIANG Yilun</p>

Notre programme sert a implementer un shell dans le système Linux.  Pour gerer les commandes qui s'exécutent dans un pipe, nous utilisons un tableau de deux dimensions pour mettre les commandes dans une sequence(ligne : les commandes, colonne: entree/sortie/erreur.) A chaque iteration, le programme lit les commandes et redirecte l'entree standard de la commande suivante vers le resultat de la commande courante. On peut mettre '&' dans le queue d'une ligne de commande pour que celle-ci marche dans l'arriere-plan. Le traitant de SIGCHILD sert a consommer toutes les commandes dans l'arriere-plan afin d'eviter la zone de zombie. Le jeu de testes, incluant les commandes simples et compliquees, est fourni pour tester notre shell. 

# 