## Temperature fablab

Le script python 'temperature.py' lit la température et l'humidité actuelle et l'ajoute à un fichier csv, 
puis crée une page html présentant un graphique de la température et l'humidité.

Le script nécessite le package adafruit-circuitpython-ahtx0, qui peut être installé par pip.

La page html utilise la librairie plotly JS (https://cdn.plot.ly/plotly-latest.min.js).

TODO: limiter la taille du csv pour avoir un historique sur quelques semaines.
