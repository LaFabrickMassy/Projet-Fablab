from tkinter import *
import random

Lab_cellnb_x = 18
Lab_cellnb_y = 18
wall_thickness = 6
cell_width=20

###########################################################
def NoneMatrix(n, m):
    reslist = []
    for i in range(0,n):
        c = [None] * m
        reslist.append(c)
    return reslist

###########################################################
#
###########################################################
class Graph:

    #######################################################
    def __init__(self, n):
        self.n = n
        self.CleanUpEdges()
        
    #######################################################
    def CleanUpEdges(self):
        self.edges = [None] * self.n
        for i in range(self.n):
            self.edges[i] = set()
        
    #######################################################
    def AddEdge(self, a, b):

        self.edges[a].add(b)
        self.edges[b].add(a)

    #######################################################
    def RemoveEdge(self, a, b):
    
        self.edges[a].remove(b)
        self.edges[b].remove(a)
        

    #######################################################
    # ConnectedComponent
    # Return the set of nodes connected to a
    #######################################################
    def ConnectedComponentOfNode(self, a):

        cc = set() # the connected component
        seen = [False] * self.n # True if node i have been seen
        tosee= [False] * self.n # True if node is to be explored
        
        tosee[a] = True
        done = False
        
        while not done:
            if True in tosee:
                x = tosee.index(True)
                for y in self.edges[x]:
                    cc.add(y)
                    if not seen[y]:
                        tosee[y] = True
                seen[x] = True
                tosee[x] = False
            else:
                done = True
        
        return cc
                
    #######################################################
    # ConnectedComponents
    # Return a list of sets of connected nodes
    #######################################################
    def ConnectedComponents(self):

        cc_list = []
        seen = [False] * self.n     # True if node i have been seen
        toexplore= [False] * self.n # True if node is to be explored
        
        while False in seen:
            x = seen.index(False)
            toexplore[x] = True
            cc = set() # the connected component
            cc_list.append(cc)
            # explore nodes
            while True in toexplore:
                y = toexplore.index(True)
                toexplore[y] = False
                if not seen[y]:
                    cc.add(y) 
                    seen[y] = True
                    # set connected nodes to explore
                    for z in self.edges[y]:
                        toexplore[z] = True
                    
        return cc_list
                
    #######################################################
    # ShortestPath : find shortest path between nodes a and b
    # return a list of nodes 
    #######################################################
    def ShortestPath(self, a, b):
    
        # array of distances : entries are 2-uples (preceding node, distance to a) 
        distance = [None]*self.n
        # list of nodes to explore
        tosee = [None]*self.n
        
        distance[a] = (a, 0)
        tosee[a] = True
        while True in tosee:
            x = tosee.index(True)
            d = distance[x][1] # current distance from a to x
            for y in self.edges[x]:
                if distance[y] is None:
                    # node never seen
                    distance[y] = (x, d+1)
                    tosee[y] = True
                elif distance[y][1] > d+1:
                    # node already seen with a longer path
                    distance[y] = (x, d+1)
                else: 
                    # node already seen with a shorter path, no need to update path
                    pass
                if y == b:
                    # no need to explore from b
                    tosee[b] = False
        
            tosee[x] = False
        
        # construct the path from a to b
        path = [b]
        prec = distance[b][0]

        while not prec == a:
            path.append(prec)
            prec = distance[prec][0]
        path.append(a)
        path.reverse()

        return path
      
###########################################################
#
###########################################################
class Labyrinth:
    
    #######################################################
    def __init__(self, cellnb_x, cellnb_y):
    
        self.cellnb_x = cellnb_x
        self.cellnb_y = cellnb_y
        
        self.generated = False
        self.density = 0
        self.learned = False
        
        self.wall_density = 0
        self.tried_density = 0
        
        self.name = "unnamed"
        
        self.graph = Graph(cellnb_x*cellnb_y)
        
        self.CleanUp()

    #######################################################
    def ij2n(self, i, j):
    
        return i*self.cellnb_y + j
        
    #######################################################
    def n2ij(self, n):
    
        i = n // self.cellnb_y
        j = n % self.cellnb_y
        return (i,j)
        
    #######################################################
    def AddVWall(self, i, j):

        self.VWalls[self.ij2n(i,j)] = True
        if i > 0:
            self.graph.RemoveEdge(self.ij2n(i,j), self.ij2n(i-1, j))

    #######################################################
    def AddHWall(self, i, j):

        self.HWalls[self.ij2n(i,j)] = True
        if j > 0:
            self.graph.RemoveEdge(self.ij2n(i,j), self.ij2n(i, j-1))

    #######################################################
    def RemoveVWall(self, i, j):

        if i > 0 :
            self.VWalls[self.ij2n(i,j)] = False
            #print(f"Lab.RemoveVWall ({i-1},{j})-({i},{j})")
            self.graph.AddEdge(self.ij2n(i,j), self.ij2n(i-1, j))

    #######################################################
    def RemoveHWall(self, i, j):

        if j > 0:
            self.HWalls[self.ij2n(i,j)] = False
            #print(f"Lab.RemoveHWall ({i},{j-1})-({i},{j})")
            self.graph.AddEdge(self.ij2n(i,j), self.ij2n(i, j-1))

    #######################################################
    def CleanUp(self):
        

        # initialise walls
        self.HWalls = [None] * self.cellnb_x*self.cellnb_y
        self.VWalls = [None] * self.cellnb_x*self.cellnb_y

        # initialise graph edges
        self.graph.CleanUpEdges()
        
        # define border walls on 1st line and column
        for i in range(0, self.cellnb_x):
            self.AddHWall(i, 0)
        for j in range(0, self.cellnb_y):
            self.AddVWall(0, j)
            
        self.walls_touched = self.cellnb_x + self.cellnb_y
        
        # set edges for all adjacent cells
        for i in range(0, self.cellnb_x):
            for j in range(0, self.cellnb_y):
                #print(f"Lab.CleanUp Add edges from ({i},{j})")
                if i < self.cellnb_x-1 :
                    self.graph.AddEdge(self.ij2n(i,j), self.ij2n(i+1,j))
                if j < self.cellnb_y-1 :
                    self.graph.AddEdge(self.ij2n(i,j), self.ij2n(i,j+1))
        
    #######################################################
    # ComputeGraphEdges
    # Set graph edges from walls definition
    #######################################################
    def ComputeGraphEdges(self):

        # clear graph edges
        self.graph.CleanUpEdges()
        
        for i in range(0, self.cellnb_x):
            for j in range(0, self.cellnb_y):
                ij = self.ij2n(i,j)
                # vertical wall
                #if i > 0 and (self.walls_v[i][j] is None or self.walls_v[i][j] == False):
                if i > 0 and (self.VWalls[self.ij2n(i,j)] is None or self.VWalls[self.ij2n(i,j)] == False):
                    ij2 = self.ij2n(i-1,j)
                    self.graph.AddEdge(ij, ij2)
                # horizontal wall
                #if j > 0 and (self.walls_h[i][j] is None or self.walls_h[i][j] == False):
                if j > 0 and (self.HWalls[self.ij2n(i,j)] is None or self.HWalls[self.ij2n(i,j)] == False):
                    ij2 = self.ij2n(i,j-1)
                    self.graph.AddEdge(ij, ij2)
            
    #######################################################
    # ConnectedComponents
    # Return list connected components
    #######################################################
    def ConnectedComponents(self):
        
        graph_cc_list = self.graph.ConnectedComponents()
        cc_list = []
        
        for graph_cc in graph_cc_list:
            cc = set()
            for ij in graph_cc:
                cc.add(self.n2ij(ij))
            cc_list.append(cc)
        
        return cc_list
        
    #######################################################
    # count density of walls set and tried
    # density is the number of walls / total possible walls
    # border walls are excluded in count as they are always on
    # 
    # @return set density
    # @return tried density
    #######################################################
    def WallDensity(self):
    
        wall_nb = 0
        wall_tried = 0

        for i in range(0, self.cellnb_x):
            for j in range(0, self.cellnb_y):
                if self.HWalls[self.ij2n(i,j)] is not None:
                    wall_tried += 1
                    if self.HWalls[self.ij2n(i,j)] == True:
                        wall_nb += 1
                if self.VWalls[self.ij2n(i,j)] is not None:
                    wall_tried += 1
                    if self.VWalls[self.ij2n(i,j)] == True:
                        wall_nb += 1

        totalnb = 2*self.cellnb_x*self.cellnb_y

        self.wall_density = wall_nb/totalnb
        self.tried_density = wall_tried/totalnb

    #######################################################
    def GenerateRandom_Init(self):

        ###################################################
        # Generate Start cell walls, output is randomly to the right or top
        if random.randint(0, 1) == 0: # 1st cell out is on the right
            self.RemoveVWall(1,0)
            # set other wall
            self.AddHWall(0,1)
            
        else: # 1st cell out is on the top
            self.RemoveHWall(0,1)
            # set other wall
            self.AddVWall(1,0)
         
        self.walls_touched = self.walls_touched +2
        
        ###################################################
        # Generate End cell
        done = False
        while not done:
            i = random.randint(1, self.cellnb_x-1)
            j = random.randint(1, self.cellnb_y-1)
            if (i > 1) and (j > 1):
                done = True
                
        # no wall around end edge in 4 directions
        self.RemoveHWall(i-1, j)
        self.RemoveHWall(i, j)
        self.RemoveVWall(i, j-1)
        self.RemoveVWall(i, j)
        self.walls_touched = self.walls_touched +4

        self.AddHWall(i-1, j-1)
        self.AddHWall(i  , j-1)
        self.walls_touched = self.walls_touched + 2

        if j < self.cellnb_y - 1:
            self.AddHWall(i-1, j+1)
            self.AddHWall(i,   j+1)
            self.walls_touched = self.walls_touched + 2

        self.AddVWall(i-1, j-1)
        self.AddVWall(i-1, j)
        self.walls_touched = self.walls_touched + 2

        if i < self.cellnb_x - 1:
            self.AddVWall(i+1, j-1)
            self.AddVWall(i+1, j)
            self.walls_touched = self.walls_touched + 2

        # select the output by removing a closing wall
        if random.randint(0,1) == 0:
            # vertical output
            # output cannot be on external walls
            if i == 1: # output on right 
                ii = i+1
                ei = i-1 # end cell
            elif i == self.cellnb_x-1: # output on left
                ii = i-1
                ei = i # end cell
            else: # normal case, interior end cell 
                if random.randint(0,1) == 0:
                    # output on left
                    ii = i-1 
                    ei = i # end cell
                else: 
                    # output on right
                    ii = i+1
                    ei = i-1
            if random.randint(0,1) == 0:
                # output up
                jj = j
                ej = j-1
            else:
                # output down
                jj = j-1 
                ej = j
            self.RemoveVWall(ii, jj)
        else:
            # horizontal output
            # output cannot be on external walls
            if j == 1: # output on top 
                jj = j+1
                ej = j-1
            elif j == self.cellnb_y-1:
                jj = j-1 # output on bottom
                ej = j
            else: # normal case
                if random.randint(0,1) == 0:
                    # output on bottom
                    jj = j-1 
                    ej = j # end cell
                else: 
                    # output on top
                    jj = j+1
                    ej = j-1
            if random.randint(0,1) == 0:
                # output left
                ii = i-1 
                ei = i
            else:
                # output right
                ii = i 
                ei = i-1
            self.RemoveHWall(ii, jj)
        
        self.end_cell = (ei,ej)
        
        self.WallDensity()

        return self.tried_density
        
    #######################################################
    def GenerateRandom_AddWall(self):
    
        self.WallDensity()
        if self.tried_density >= 1:
            # all walls are tried, cancel generation
            return self.tried_density
            
        cc_list = self.ConnectedComponents()
        if len(cc_list) > 1:
            # 2 or more connected components, cancel generation
            return self.tried_density
        
        done = False
        count = 0
        while not done:
            count += 1
            if random.randint(0,1) == 0:
                # search a free vertical wall
                free_walls_idx = [i for i in range(len(self.VWalls)) if self.VWalls[i] is None]
                if len(free_walls_idx) > 0:
                    n = free_walls_idx[random.randint(0,len(free_walls_idx)-1)]
                    (i,j) = self.n2ij(n)
                    
                    self.AddVWall(i,j)
                    self.last_wall = [i,j, False]

                    cc_list = self.graph.ConnectedComponents()
                    if len(cc_list) == 1:
                        done = True
                    elif len(cc_list) > 2:
                        print("ERROR")
                        done = True
                    else:
                        self.RemoveVWall(i,j)
                    self.walls_touched = self.walls_touched + 1
                        
            else:
                # search a free horizontal wall
                free_walls_idx = [i for i in range(len(self.HWalls)) if self.HWalls[i] is None]
                if len(free_walls_idx) > 0:
                    n = free_walls_idx[random.randint(0,len(free_walls_idx)-1)]
                    (i,j) = self.n2ij(n)

                    self.AddHWall(i,j)
                    self.last_wall = [i,j, True]
                    cc_list = self.graph.ConnectedComponents()

                    if len(cc_list) == 1:
                        done = True
                    elif len(cc_list) > 2:
                        print("ERROR")
                        done = True
                    else:
                        self.RemoveHWall(i,j)
                    self.walls_touched = self.walls_touched + 1
                    
            self.WallDensity()
            if self.tried_density == 1:
                done = True
                
        return self.tried_density

    #######################################################
    def ShortestPath(self, a, b):

        an = self.ij2n(a[0], a[1])
        bn = self.ij2n(b[0], b[1])
        
        gpath = self.graph.ShortestPath(an, bn)
        
        path = []
        for x in gpath:
            xij = self.n2ij(x)
            path.append(xij)
            
        return path
        
###########################################################
#
###########################################################
class HMI():
    button_width=80
    button_height=50

    default_cell_pxsize = 20
    default_wall_pxthickness = 6    
    
    #######################################################
    def __init__(self, lab):
    
        self.lab = lab
        
        self.tkapp = Tk()
        
        self.saved = False
        self.status = ""

        # compute display sizes        
        screen_width = self.tkapp.winfo_screenwidth()
        screen_height = self.tkapp.winfo_screenheight()

        window_height = screen_height*0.8
        self.cell_pxsize = int((window_height-wall_thickness)/self.lab.cellnb_y)
        
        self.draw_width = (self.lab.cellnb_x*self.cell_pxsize) + HMI.default_wall_pxthickness +4
        self.draw_height = (self.lab.cellnb_y*self.cell_pxsize) + HMI.default_wall_pxthickness  +4

        self.window_width = self.draw_width + HMI.button_width

        ##### Define menu
        self.tkmenu = Menu(self.tkapp)
        # File : Save, Load | Exit
        self.tkmenu_file = Menu(self.tkmenu)
        self.tkmenu.add_cascade(label="File", menu = self.tkmenu_file)
        self.tkmenu_file.add_command(label = "Export", command = self.Menu_File_Export)
        self.tkmenu_file.add_command(label = "Import", command = self.Menu_File_Import)
        self.tkmenu_file.add_separator()
        self.tkmenu_file.add_command(label = "Exit", command = self.tkapp.destroy)
        # Labyrinth : New, Change size | Explore, Run
        self.tkmenu_lab = Menu(self.tkmenu)
        self.tkmenu.add_cascade(label="Labyrinth", menu = self.tkmenu_lab)
        self.tkmenu_lab.add_command(label = "New", command = self.Menu_Lab_NewLab)
        self.tkmenu_lab.add_command(label = "Change size", command = self.Menu_Lab_ChangeSize)
        self.tkmenu_file.add_separator()
        self.tkmenu_lab.add_command(label = "Explore", command = self.Menu_Lab_Explore)
        self.tkmenu_lab.add_command(label = "Explore", command = self.Menu_Lab_Run)
        
        # show menu
        self.tkapp.config(menu=self.tkmenu)
        
        self.tkapp.geometry("{}x{}".format(self.draw_width, self.draw_height))

        self.canvas = Canvas(self.tkapp, width=self.draw_width, height=self.draw_height, background="white")
        #self.canvas.place(width=self.draw_width, height=self.draw_height, x=0, y=0)
        #self.canvas = Canvas(self.tkapp, background="white")
        self.canvas.place(x=0, y=0)
        
        self.tkapp.bind('<Key>', self.KeyPressed)
        
        self.SetWindowTitle()

    #######################################################
    def NewLab(self):
    
        self.status = "Generating..."
        self.SetWindowTitle()
        self.lab.CleanUp()
        wall_density = self.lab.GenerateRandom_Init()
        self.DrawLab()                        
        #self.tkapp.update()
        while wall_density < 1:
            wall_density = self.lab.GenerateRandom_AddWall()
            [i,j,f]=self.lab.last_wall
            self.DrawWall(i, j, f)
            self.SetWindowTitle()
            self.tkapp.update()
        self.DrawLab()                        
        self.status = ""
        self.SetWindowTitle()
    

    #######################################################
    def NewLab_Init(self):
    
        self.lab.CleanUp()
        wall_density = self.lab.GenerateRandom_Init()
        print(f"Initial generation : density={wall_density}")
        
        self.SetWindowTitle()
        self.DrawLab()

    #######################################################
    def NewLab_Continue(self):
    
        wall_density = self.lab.GenerateRandom_AddWall()
        self.SetWindowTitle()
        self.DrawLab()                        

    #######################################################
    def SetWindowTitle(self):
    
        title = self.lab.name
        if not self.saved:
            title = title + "*"
        title = title + " " + str(self.lab.cellnb_x) + "x" + str(self.lab.cellnb_y)
        if self.lab.wall_density > 0:
          title = title + " ({}%)".format(int(100*self.lab.wall_density))
        title = title + " " + self.status
        self.tkapp.title(title)
        
    #######################################################
    def DrawLine(self, ai, aj, bi, bj, color="#000000"):

        ax = ai * self.cell_pxsize + HMI.default_wall_pxthickness/2 +2 + self.cell_pxsize/2
        ay = aj * self.cell_pxsize + HMI.default_wall_pxthickness/2 +2 + self.cell_pxsize/2
        bx = bi * self.cell_pxsize + HMI.default_wall_pxthickness/2 +2 + self.cell_pxsize/2
        by = bj * self.cell_pxsize + HMI.default_wall_pxthickness/2 +2 + self.cell_pxsize/2

        self.canvas.create_rectangle(ax, self.draw_height-ay, bx, self.draw_height-by, fill=color)

    #######################################################
    def DrawWall(self, i, j, h_flag, color="#000000"):
    
        x = i * self.cell_pxsize + HMI.default_wall_pxthickness/2 +2
        y = j * self.cell_pxsize + HMI.default_wall_pxthickness/2 +2

        if h_flag: # horizontal wall 
            self.canvas.create_rectangle(x, self.draw_height-(y-HMI.default_wall_pxthickness/2), x+self.cell_pxsize, self.draw_height-(y+HMI.default_wall_pxthickness/2), fill=color)
        else:
            self.canvas.create_rectangle(x-HMI.default_wall_pxthickness/2, self.draw_height-(y), x+HMI.default_wall_pxthickness/2, self.draw_height-(y+self.cell_pxsize), fill=color)
        
    #######################################################
    def DrawCell(self, i, j, color = "#cccccc"):
    
        x = i * self.cell_pxsize + HMI.default_wall_pxthickness +5
        y = j * self.cell_pxsize + HMI.default_wall_pxthickness +5
        
        self.canvas.create_rectangle(x, self.draw_height-(y), x+self.cell_pxsize-HMI.default_wall_pxthickness*2, self.draw_height-(y+self.cell_pxsize-HMI.default_wall_pxthickness*2), fill=color, outline=color)
        
    #######################################################
    def DrawLab(self):

        # draw labyrinth ground
        x0 = HMI.default_wall_pxthickness/2
        y0 = HMI.default_wall_pxthickness/2
        x1 = self.lab.cellnb_x*self.cell_pxsize + HMI.default_wall_pxthickness/2
        y1 = self.lab.cellnb_y*self.cell_pxsize + HMI.default_wall_pxthickness/2
        self.canvas.create_rectangle(x0, y0, x1, y1, fill="#cccccc", outline="#cccccc")

        # draw start and end cell
        self.DrawCell(0,0, color = "green")
        self.DrawCell(self.lab.end_cell[0], self.lab.end_cell[1], color = "red")

        # draw interior walls
        for i in range(0, self.lab.cellnb_x):
            for j in range(0, self.lab.cellnb_y):
                if (i > 0) or (j > 0): # only interior walls
                    if self.lab.HWalls[self.lab.ij2n(i,j)] == True:
                        self.DrawWall(i, j, True)
                    if self.lab.VWalls[self.lab.ij2n(i,j)] == True:
                        self.DrawWall(i, j, False)
        # draw border walls
        for i in range(0, self.lab.cellnb_x):
            self.DrawWall(i, 0, True)
            self.DrawWall(i, self.lab.cellnb_y, True)
        for j in range(0, self.lab.cellnb_y):
            self.DrawWall(0, j, False)
            self.DrawWall(self.lab.cellnb_x, j, False)
            
    #######################################################
    def Solve(self):
    
        path = self.lab.ShortestPath((0,0), self.lab.end_cell)
        for i in range(len(path)-1):
            (ai, aj) = path[i]
            (bi, bj) = path[i+1]
            self.DrawLine(ai, aj, bi, bj)
            
        self.status = f"solved in {len(path)-1} steps"
        self.SetWindowTitle()

    #######################################################
    def mainloop(self):
    
        self.tkapp.mainloop()

    #######################################################
    # Menu functions
    #######################################################

    #######################################################
    def Menu_File_Export(self):
        pass
    
    #######################################################
    def Menu_File_Import(self):
        pass
    
    #######################################################
    def Menu_Lab_NewLab(self):
        self.NewLab()
    
    #######################################################
    def Menu_Lab_ChangeSize(self):
        pass
    
    #######################################################
    def Menu_Lab_Explore(self):
        pass
    
    #######################################################
    def Menu_Lab_Run(self):
        pass
    
    #######################################################
    #
    #######################################################
    
    #######################################################
    def KeyPressed(self, event):
        key = event.char
        if key == 'q':
            self.tkapp.quit()
        elif key == 'N': # to test step by step maze generation
            self.NewLab_Init()
        elif key == 'C': # to test step by step maze generation
            self.NewLab_Continue()
        elif key == 'n':
            self.NewLab()
        elif key == 's':
            self.Solve()
        else:
            pass
            #print(f"<{key}>")
            #print(int(key))
        
lab = Labyrinth(Lab_cellnb_x, Lab_cellnb_y)
hmi = HMI(lab)
hmi.mainloop()

