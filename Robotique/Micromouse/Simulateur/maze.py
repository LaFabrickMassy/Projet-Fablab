import random
from graph import *

###########################################################
#
###########################################################
class Maze:
    
    cell_width = 200 # cell width in mm, including walls
    wall_width = 12  # wall width in mm
    
    #######################################################
    def __init__(self, cellnb_x, cellnb_y):
    
        self.cellnb_x = cellnb_x
        self.cellnb_y = cellnb_y
        self.end_cell = None
        
        self.generated = False
        self.density = 0
        
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
            self.graph.AddEdge(self.ij2n(i,j), self.ij2n(i-1, j))

    #######################################################
    def RemoveHWall(self, i, j):

        if j > 0:
            self.HWalls[self.ij2n(i,j)] = False
            self.graph.AddEdge(self.ij2n(i,j), self.ij2n(i, j-1))

    #######################################################
    def CleanUp(self):
        

        # initialise walls
        self.HWalls = [None] * self.cellnb_x*self.cellnb_y
        self.VWalls = [None] * self.cellnb_x*self.cellnb_y

        # initialise graph edges
        self.graph.CleanUpEdges()
        
        # define border walls on 1st line and olumn only
        for i in range(0, self.cellnb_x):
            self.AddHWall(i, 0)
        for j in range(0, self.cellnb_y):
            self.AddVWall(0, j)
            
        self.walls_touched = self.cellnb_x + self.cellnb_y
        
        # set edges for all adjacent cells
        for i in range(0, self.cellnb_x):
            for j in range(0, self.cellnb_y):
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
            self.start_H = True
            
        else: # 1st cell out is on the top
            self.RemoveHWall(0,1)
            # set other wall
            self.AddVWall(1,0)
            self.start_H = False
         
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
        
        self.generated = True

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
        
