import pygame
import os
import time

# Initialisation de pygame
pygame.init()

# Définir la taille de l'écran en fonction de la résolution actuelle
screen = pygame.display.set_mode((0, 0), pygame.FULLSCREEN)
w = screen.get_rect().width
h = screen.get_rect().height

working_dir = "C:\\tmp\\working"

def displayImg(image_path):
    image = pygame.image.load(image_path)

    # Redimensionner l'image pour s'adapter à la taille de l'écran
    img_w = image.get_rect().width
    img_h = image.get_rect().height
    k_w = float(img_w) / float(w)
    k_h = float(img_h) / float(h)
    factor = k_w if (k_w > k_h) else k_h
   
    image = pygame.transform.scale_by(image, factor=factor)
    img_w = image.get_rect().width
    img_h = image.get_rect().height

    # Afficher l'image en plein écran
    screen.blit(image, ((w-img_w)/2, (h-img_h)/2))
    pygame.display.flip()
    
def fileListChanged(list1, list2):
    l1_sorted = sorted(list1)
    l2_sorted = sorted(list2)
    return l1_sorted != l2_sorted


def getImgList():
    return [f for f in os.listdir(working_dir) if os.path.isfile(os.path.join(working_dir, f)) and f.lower().endswith(".png")]
    
file_list = getImgList()
print(file_list)
index = 0

while True:
    new_file_list = getImgList()
    
    if fileListChanged(new_file_list, file_list):
        file_list = new_file_list
        index = 0
        
    if len(file_list) > 0:
        file_name = file_list[index]
            
        image_path = os.path.join(working_dir, file_name)
        displayImg(image_path)
        #iterate
        index = (index + 1) if (index + 1) < len(file_list)  else 0
    
    # replace this with the wait for the gpio input 
    time.sleep(2)
    
for image_path in [os.path.join(working_dir, f) for f in os.listdir(working_dir) if os.path.isfile(os.path.join(working_dir, f)) ]:
    if(not image_path.lower().endswith(".png")):
        continue
    # Charger l'image
    
    displayImg(image_path)

    # Attendre jusqu'à ce que l'utilisateur ferme la fenêtre
    running = True
    time.sleep(5)
    # while running:
    #     for event in pygame.event.get():
    #         print(f"event: {event}")
    #         if event.type == pygame.QUIT:
    #             running = False



# Quitter pygame
pygame.quit()