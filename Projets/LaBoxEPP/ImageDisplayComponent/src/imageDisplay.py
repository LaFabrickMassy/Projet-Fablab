 
import os.path
from tkinter import Tk, Button, Canvas, PhotoImage
from PIL import Image, ImageTk

working_dir = "C:\\tmp\\working"
 
fenetre = Tk()
 
# sert a ne pas avoir la bande noir en haut et en bas de la page en fullscreen
fenetre.attributes('-fullscreen', True)


w, h = fenetre.winfo_screenwidth(), fenetre.winfo_screenheight()
for file in [os.path.join(working_dir, f) for f in os.listdir(working_dir) if os.path.isfile(os.path.join(working_dir, f) )]:
    print(f"fichier; {file} {file.lower().endswith('.png')}")

for file in [os.path.join(working_dir, f) for f in os.listdir(working_dir) if os.path.isfile(os.path.join(working_dir, f)) ]:
    print(f"fichier; {file}")
    if not file.lower().endswith('.png'):
        continue
    

    image = Image.open(file) # imageTest fait 270 x 200
    bbox = image.getbbox()
    img_w= bbox[2]- bbox[0]
    img_h = bbox[3]-bbox[1]
    k_w = float(img_w) / float(w)
    k_h = float(img_h) / float(h)
    factor = 1
    if(k_w > k_h):
        factor = k_w
    else:
        factor = k_h
    i_w = int(img_w / factor)
    i_h = int(img_h / factor)
    
    image = image.resize((i_w, i_h))
    
    # print(image) # <PIL.Image.Image image mode=RGBA size=1920x1080 at 0x43D5AB0>
    
    photo = ImageTk.PhotoImage(image)
    
    # pour que le canvas n'ai pas de bourdure qui reste autour
    can = Canvas(fenetre, highlightthickness=0)
    
    can.create_image((w-i_w)/2, (h-i_h)/2, anchor='nw', image=photo)
    can.pack(fill='both', expand=1)
    input()

 
Button(can, text=f"Quitter {str(image.getbbox())}, {img_w}, {img_h}, {factor} ", command=fenetre.destroy).pack()
 
fenetre.mainloop()