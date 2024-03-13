Oui, il est tout à fait possible d'afficher des images avec Python sur un Raspberry Pi Zero exécutant Raspberry Pi OS Lite. Pour cela, vous pouvez utiliser la bibliothèque Python Pillow (anciennement connue sous le nom de PIL - Python Imaging Library). 
Pillow offre une facilité d'utilisation pour manipuler des images en Python.

Voici comment vous pouvez installer Pillow et afficher une image dans un script Python :

Installer Pillow :
Assurez-vous d'avoir pip installé. Si ce n'est pas le cas, vous pouvez l'installer avec la commande suivante :

```bash
Copy code
sudo apt-get update
sudo apt-get install python3-pip
```
Ensuite, installez Pillow avec :

```bash
Copy code
pip3 install pillow
```

Écrire un script Python pour afficher une image :  
Créez un fichier Python, par exemple afficher_image.py, et ajoutez le code suivant :  

```python
from PIL import Image
from PIL import ImageTk  # Facultatif, pour afficher des images dans une interface graphique Tkinter

# Spécifiez le chemin de l'image
chemin_image = "chemin/vers/votre/image.jpg"

# Ouvrir l'image avec Pillow
image = Image.open(chemin_image)

# Afficher l'image
image.show()

# Pour Tkinter (interface graphique), vous pouvez utiliser ceci :
# image_tk = ImageTk.PhotoImage(image)
# Créez une fenêtre Tkinter et ajoutez une étiquette pour afficher l'image
# Utilisez cette étiquette comme composant dans votre interface graphique

# Attente de la fermeture de la fenêtre de l'image (utile si vous n'utilisez pas Tkinter)
input("Appuyez sur Enter pour quitter.")
```
Exécuter le script :  
Exécutez le script Python à l'aide de la commande suivante :  

```bash
python3 afficher_image.py
```
Assurez-vous d'adapter le chemin de l'image dans le script à l'emplacement réel de votre image.  

Veuillez noter que image.show() ouvrira l'image avec l'application d'affichage d'images par défaut du Raspberry Pi. 
Si vous souhaitez intégrer l'affichage d'images dans une application plus complexe ou une interface utilisateur, 
vous pouvez également explorer l'utilisation de bibliothèques graphiques telles que Tkinter pour créer des interfaces utilisateur avec des composants d'image.
