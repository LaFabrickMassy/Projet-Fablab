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
      
