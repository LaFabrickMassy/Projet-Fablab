## Telemetry

La classe `Telemetry` gère la communication du robot avec un navigateur web. Elle reçoit les ordres et retourne les mesures et informations utiles. Elle doit être dérivée (c'est une classe virtuelle pure) pour définir les callbacks. Une seule instanciation d'une seule classe dérivée est autorisée.

Il faut instancier l'unique objet dérivé de `Telemetry` dans le `setup` Arduino après avoir initialisé la classe `Serial`. Tous les paramètres du constructeur ont une valeur par défaut. Mais il est possible de les préciser pour changer les paramètre du WIFI.

Il faut appeler la fonction `loop` de l'objet `Telemetry` dans le `loop` Arduino.

L'utilisation de la librairie `Telemetry.h` requière l'installation de la librairie `ArduinoWebsockets` à l'aide du Gestionnaire de Librairies de l'IDE Arduino.

Le fichier de test de la librairie `Telemetry` qui se trouve dans le répertoire `TestTelemetry` permet non seulement de faire la non régression de la librairie, mais peut aussi servir d'exemple d'utilisation.

