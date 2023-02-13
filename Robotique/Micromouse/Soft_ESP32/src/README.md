# Logiciel ESP32 Micromouse

Code embarqué pour le micromouse.

## Prérequis

- Uploader système de fichiers ESP32 (voir https://randomnerdtutorials.com/install-esp32-filesystem-uploader-arduino-ide/)


## Compilation/Installation

Les fichiers html/js/css doivent être mis dans dans la flash pour pouvoir être lus par le serveur web. Ces fichiers sont dans le répertoire data/. 
Le code est compilé et uploadé sur l'ESP comme pour un Arduino, mais les fichiers sont uploadés différemment.

Choisir un "Partition Scheme" incluant SPIFFS (par exemple Default 4MB with spiffs (1.2MB APP/1.5MB SPIFFS).
Uploader le contenu du répertoire data/ sur la flash de l'ESP32 par le menu "Outils/ESP32 Sketch Data Upload".



