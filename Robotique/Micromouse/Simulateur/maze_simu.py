from tkinter import *
from tkinter.messagebox import *
import random
import math
from maze import *
from robot import *
from hmi import *

default_cellnb_x = 9
default_cellnb_y = 9


###########################################################
def NoneMatrix(n, m):
    reslist = []
    for i in range(0,n):
        c = [None] * m
        reslist.append(c)
    return reslist

hmi = HMI(default_cellnb_x, default_cellnb_y)
hmi.mainloop()
