import pygame
import os
import time
from os import environ
import logging
import signal
import RPi.GPIO as GPIO

BUTTON_GPIO = 4

button_pressed = False

logging.basicConfig(filename="/var/log/laboxepp/eppdisplay.log")

# Initialisation de pygame
pygame.init()

# Définir la taille de l'écran en fonction de la résolution actuelle
screen = pygame.display.set_mode((0, 0), pygame.FULLSCREEN)
w = screen.get_rect().width
h = screen.get_rect().height
msg = f"Screen size: {w}x{h}"
print(msg)
logging.info(msg)

working_dir = environ.get("EPP_IMG_DIR")
msg=f"Working directory = {working_dir}"
print(msg)
logging.info(msg)

if working_dir is None:
    working_dir = "/var/laboxepp/"
    if not os.path.exists(working_dir):
        os.mkdirs(working_dir) 

def displayImg(image_path):
    image = pygame.image.load(image_path)

    # Redimensionner l'image pour s'adapter à la taille de l'écran
    init_w = image.get_rect().width
    init_h = image.get_rect().height
    k_w = float(init_w) / float(w)
    k_h = float(init_h) / float(h)
    factor = k_w if (k_w > k_h) else k_h
   
    image = pygame.transform.scale(image, (int(init_w/factor), int(init_h/factor)))
    #image = pygame.transform.scale_by(image, factor=factor)
    img_w = image.get_rect().width
    img_h = image.get_rect().height
    
    print(f"Initial size: {init_w}x{init_h}, transformed: {img_w}x{img_h}")

    # Afficher l'image en plein écran
    screen.blit(image, ((w-img_w)/2, (h-img_h)/2))
    pygame.display.flip()
  
def button_callback(gpio):
    global button_pressed
    time.sleep(0.05) # debounce
    if GPIO.input(gpio) == GPIO.LOW:
        print("Button pressed")
        logging.info("Button pressed")
        button_pressed = True
    
def initButton():
    GPIO.setmode(GPIO.BCM) # numbers as gpio value
    GPIO.setup(BUTTON_GPIO, GPIO.IN, pull_up_down=GPIO.PUD_UP)
    GPIO.add_event_detect(BUTTON_GPIO, GPIO.FALLING, callback=button_callback, bouncetime=50)

    
def fileListChanged(list1, list2):
    l1_sorted = sorted(list1)
    l2_sorted = sorted(list2)
    return l1_sorted != l2_sorted


def getImgList():
    return [f for f in os.listdir(working_dir) if os.path.isfile(os.path.join(working_dir, f)) and f.lower().endswith(".png")]

def exit_display(signal, frame):
    logging.info("Exiting display on interruption signal")
    logging.shutdown()



signal.signal(signal.SIGINT, exit_display)

initButton()

file_list = getImgList()
print(file_list)
index = 0

running = True
while running:
    new_file_list = getImgList()
    
    if fileListChanged(new_file_list, file_list):
        file_list = new_file_list
        index = 0
        
    
    # replace this with the wait for the gpio input 
    if(button_pressed):
        button_pressed = False
        if len(file_list) > 0:
            file_name = file_list[index]                
            image_path = os.path.join(working_dir, file_name)
            displayImg(image_path)
            #iterate
            index = (index + 1) if (index + 1) < len(file_list)  else 0
        

    for event in pygame.event.get():
        print(f"event: {event}")
        logging.debug(f"event: {event}")
        if event.type == 2:   # key down
            running = False
        #if event.type == pygame.QUIT:
        #    running = False
    
#for image_path in [os.path.join(working_dir, f) for f in os.listdir(working_dir) if os.path.isfile(os.path.join(working_dir, f)) ]:
#    if(not image_path.lower().endswith(".png")):
#        continue
    # Charger l'image
    
#    displayImg(image_path)
#    logging(f"event: {event}")
    # Attendre jusqu'à ce que l'utilisateur ferme la fenêtre
#   running = True
 #   time.sleep(5)
    # while running:
    #     for event in pygame.event.get():
    #         print(f"event: {event}")
    #         if event.type == pygame.QUIT:
    #             running = False



# Quitter pygame
pygame.quit()
