import math
from maze import *

###########################################################
# Compute the distance between a ray, defined by a point p and
# an angle alpha, and an horizontal segment [a,b]
# distance is math.inf if no intersection
#
# @param px, py coordinates of point p
# @param alpha angle of ray from x axis
# @param ax, ay coordinates of point a
# @param bx x coordinare of point b (here, by=ay)
#
# @return [distance, tx, ty] where t is the intersection point
#         None if no intersection
#
###########################################################
def Distance2HSegment(px, py, alpha, ax, ay, bx, by):

    #### DEBUG ####
    #print(f"Distance2HSegment {px},{py},{alpha},{ax},{ay},{bx},{ay}")
    
    # Normalise alpha in ]-pi,pi]
    alpha = math.fmod(alpha, 2*math.pi)    
    # here, alpha is in ]-2pi, 2pi[
    if alpha < 0: 
        alpha = alpha + 2*math.pi
    # here, alpha is in [0, 2pi[
    if alpha > math.pi:
        alpha = alpha - 2*math.pi
    # finished, alpha is now in ]-pi, pi]
    
    # set default results
    distance = math.inf
    tx = None
    ty = None
    
    # if b is to the left of a, swap a and b so ax < bx
    if bx < ax:
        cx = ax
        ax = bx
        bx = cx
        
    if alpha == 0:
        # intersection only if py == ay
        if py == ay:
            # intesection only if py < by
            if px < bx:
                if px > ax:
                    # p is on [a,b]
                    distance = 0
                    tx = px
                    ty = py
                else:
                    # p on the line, to the left of a
                    # intersection is a
                    distance = ay-py
                    tx = ax
                    ty = ay
            else:
                pass
        else:
            pass
    elif alpha == -math.pi:
        # intersection only if py == ay
        if py == ay:
            # intesection only if px > ax
            if px > ax:
                if px < bx:
                    # p is on [a,b]
                    distance = 0
                    tx = px
                    ty = py
                else:
                    # p on the line, to the right of b
                    # intersection is b
                    distance = py - by
                    tx = bx
                    ty = ay
            else:
                pass
        else:
            pass
    elif (py > ay) and (alpha > 0):
        # p is above and facing up
        pass
    elif (py < ay) and (alpha < 0):
        # p is below and facing down
        pass
    elif (px < ax) and ((alpha > math.pi/2) or (alpha < -math.pi/2)):
        # p is to the left and facing left
        pass
    elif (px > bx) and ((alpha < math.pi/2) and (alpha > -math.pi/2)):
        # p is to the right and facing right
        pass
    else: # general case
        # o point on (a,b) with same x coordinate as p
        # The triangle (p,o,t) is square in o, 
        # so op=distance * cos(beta) and ot=distance * sin(beta)
        # with beta = pi/2-alpha 
        beta = math.pi/2-alpha
        distance = abs((ay-py)/math.cos(beta))
        tx = px + distance*math.sin(beta)
        if (ax<tx) and (tx<bx):
            ty = ay
        else:
            distance = math.inf
            tx = None

    #### DEBUG ####
    #print(f"  ==> {distance},{tx},{ty}")

    return [distance, tx, ty]
        
###########################################################
# Compute the distance between a ray, defined by a point p and
# an angle alpha, and a vertical segment [a,b]
# distance is math.inf if no intersection
#
# @param px, py coordinates of point p
# @param alpha angle of ray from x axis
# @param ax, ay coordinates of point a
# @param by y coordinare of point b (here, bx=ax)
#
# @return [distance, tx, ty] where t is the intersection point
#         None if no intersection
#
###########################################################
def Distance2VSegment(px, py, alpha, ax, ay, bx, by):

    #### DEBUG ####
    #print(f"Distance2VSegment {px},{py},{alpha},{ax},{ay},{ax},{by}")
    
    # Normalise alpha in ]-pi,pi]
    alpha = math.fmod(alpha, 2*math.pi)    
    # here, alpha is in ]-2pi, 2pi[
    if alpha < 0: 
        alpha = alpha + 2*math.pi
    # here, alpha is in [0, 2pi[
    if alpha > math.pi:
        alpha = alpha - 2*math.pi
    # finished, alpha is now in ]-pi, pi]
    
    # set result to None
    distance = math.inf
    tx = None
    ty = None
    
    # if b is under a, swap a and b so ay < by
    if by < ay:
        cy = ay
        ay = by
        by = cy
        
    if alpha == math.pi/2:
        #### DEBUG
        #print("  Case 1 : alpha=90")
        
        # intersection only if px == ax
        if px == ax:
            # intesection only if py < by
            if py < by:
                if py > ay:
                    #### DEBUG
                    #print("p is on [a,b]")
                    # p is on [a,b]
                    distance = 0
                    tx = px
                    ty = py
                else:
                    # p on the line, under a
                    # intersection is a
                    #### DEBUG
                    #print("p is on the line, under a")
                    distance = ax - px
                    tx = ax
                    ty = ay
            else:
                pass
        else:
            pass
    elif alpha == -math.pi/2:
        #### DEBUG
        #print("  Case 2: alpha=-90")

        # intersection only if px == ax
        if px == ax:
            # intesection only if py > ay
            if py > ay:
                if py < by:
                    # p is on [a,b]
                    distance = 0
                    tx = px
                    ty = py
                else:
                    # p on the line, over b
                    # intersection is b
                    distance = px - bx
                    tx = ax
                    ty = by
            else:
                pass
        else:
            pass
    elif (py > by) and (alpha > 0):
        # p is above and facing up
        
        #### DEBUG
        #print("  Case 3 : p above segment")
        
        pass
    elif (py < ay) and (alpha < 0):
        # p is below and facing down

        #### DEBUG
        #print("  Case 4 : p below segment")
        
        pass
    elif (px < ax) and ((alpha > math.pi/2) or (alpha < -math.pi/2)):
        # p is to the left and facing left

        #### DEBUG
        #print("  Case 5 : p to the left")
        
        pass
    elif (px > ax) and ((alpha < math.pi/2) and (alpha > -math.pi/2)):
        # p is to the right and facing right

        #### DEBUG
        #print("  Case 6 : p to the right")
        
        pass
    else: # general case
        # o point on (a,b) with same y coordinate as p
        # The triangle (p,o,t) is square in o, 
        # so op=distance * cos(alpha) and ot=distance * sin(alpha)
        
        distance = abs((ax-px)/math.cos(alpha))
        ty = py + distance*math.sin(alpha)

        #### DEBUG
        #print(f"  Case 7 : good case, ty={ty}")        

        if (ay<ty) and (ty<by):
        
            #### DEBUG
            #print("    Case 7a : t on segment")
            tx = ax
        else:

            #### DEBUG
            #print("    Case 7b : t not on segment")

            distance = math.inf
            ty = None

    #### DEBUG ####
    #print(f"  ==> {distance},{tx},{ty}")

    return [distance, tx, ty]

###########################################################
#
###########################################################
class Robot:

    simu_time_step = 0.1 # time step in seconds
    wheels_distance = 100 # distance between wheels, in mm

    #######################################################
    def __init__(self, maze):
    
        self.maze = maze
        
        self.current_pos_x = 0
        self.current_pos_y = 0
        self.current_heading = 0
        
        self.StartPosition()
    
    #######################################################
    #
    #######################################################
    def StartPosition(self):
        if self.maze.generated == False or self.maze.start_H:
            # 1st cell output on the right
            self.current_heading = 0
            self.current_pos_x = Robot.wheels_distance/2 + Maze.wall_width
            self.current_pos_y = (Maze.cell_width - Maze.wall_width)/2
        else:
            # 1st cell output on the top
            self.current_heading = math.pi/2
            self.current_pos_x = (Maze.cell_width - Maze.wall_width)/2
            self.current_pos_y = Robot.wheels_distance/2 + Maze.wall_width

    #######################################################
    # RunStep
    # compute new position from speed and curvature
    # speed is set for external wheel. It has main impact for high curvature values.
    # curvature is > 0 when turning right (clockwise). 
    # curvature = 0 for straight forward, +/-math.inf for on place turn
    #######################################################
    def RunStep(self, speed, curvature):
        distance = speed*self.simu_time_step
        
        if curvature == 0:
            dx = distance * math.cos(current_heading)
            dy = distance * math.sin(current_heading)
            
            self.current_pos_x += dx
            self.current_pos_y += dy
        elif curvature == math.inf: # clockwise on place rotation, change only heading

            # radius of curve circle for external wheel
            radius_ext = Robot.wheel_distance

            # rotation angle
            angle = distance / radius_ext
            
            # new heading
            self.heading = self.heading - angle

        elif curvature == -math.inf: # counterclockwise on place rotation, change only heading

            # radius of curve circle for external wheel
            radius_ext = Robot.wheel_distance

            # rotation angle
            angle = distance / radius_ext
            
            # new heading
            self.heading = self.heading + angle

        else: # general case, change heading and center position
        
            # radius or curve circle for center of robot
            radius = 1/curvature
            
            # radius of curve circle for external wheel
            radius_ext = radius + Robot.wheel_distance
            
            # unit vector pointed on heading
            u_x = math.cos(self.current_heading)
            u_y = math.sin(self.current_heading)
            
            # unit vector pointing to center of curve circle, orthogonal to u
            if curvature > 0:
                # rotate to left, otrhogonal is -pi/2 rotation
                uorth_x = u_y
                uorth_y = -u_x
            else:
                # rotate to right, otrhogonal is pi/2 rotation
                uorth_x = -u_y
                uorth_y = u_x
            
            # center of circle
            o_x = self.current_pos_x + radius * uorth_x
            o_y = self.current.pos_x + radius * uorth_y
            
            # position of external wheel
            R_x = o_x - radius_ext*u_x
            R_y = o_y - radius_ext*u_y
            
            # rotation angle
            angle = distance / radius_ext
            
            if curvature > 0:
                # new heading
                self.heading = self.heading - angle
                # new unit vector
                u_x = math.cos(self.current_heading)
                u_y = math.sin(self.current_heading)
                # new unit vector
                u_x = math.cos(self.current_heading)
                u_y = math.sin(self.current_heading)
                # new orthogonal unit vector
                uorth_x = u_y
                uorth_y = -u_x
            else:
                # new heading
                self.heading = self.heading + angle
                # new unit vector
                u_x = math.cos(self.current_heading)
                u_y = math.sin(self.current_heading)
                # new unit vector
                u_x = math.cos(self.current_heading)
                u_y = math.sin(self.current_heading)
                # new orthogonal unit vector
                uorth_x = -u_y
                uorth_y = u_x
            
            # new center position
            self.current_pos_x = o_x - radius*uorth_x
            self.current_pos_y = o_y - radius*uorth_y
            
    #######################################################
    # WallDistance_unit_H
    # return distance to a H wall in maze
    #
    #######################################################
    def WallDistance_unit_H(self, angle, i, j):
        
        # coordinates of extremities of wall
        ax = i * Maze.cell_width
        ay = j * Maze.cell_width
        bx = ax + Maze.cell_width
        by = ay
        
        # distance to top segment
        sax = ax
        say = ay + Maze.wall_width/2
        sbx = bx
        sby = by + Maze.wall_width/2
        [dmin, tx, ty] = Distance2HSegment(self.current_pos_x, self.current_pos_y, angle, sax, say, sbx, sby)
        # distance to bottom segment
        sax = ax
        say = ay - Maze.wall_width/2
        sbx = bx
        sby = by - Maze.wall_width/2
        [d, ttx, tty] = Distance2HSegment(self.current_pos_x, self.current_pos_y, angle, sax, say, sbx, sby)
        if d < dmin:
            dmin = d
            tx = ttx
            ty = tty
        # distance to left segment
        sax = ax
        say = ay + Maze.wall_width/2
        sbx = ax
        sby = ay - Maze.wall_width/2
        [d, ttx, tty] = Distance2VSegment(self.current_pos_x, self.current_pos_y, angle, sax, say, sbx, sby)
        if d < dmin:
            dmin = d
            tx = ttx
            ty = tty
        # distance to right segment
        sax = bx
        say = by + Maze.wall_width/2
        sbx = bx
        sby = by - Maze.wall_width/2
        [d, ttx, tty] = Distance2VSegment(self.current_pos_x, self.current_pos_y, angle, sax, say, sbx, sby)
        if d < dmin:
            dmin = d
            tx = ttx
            ty = tty
            
        return [dmin, tx, ty]

    #######################################################
    # WallDistance_unit_V
    # return distance to a H wall in maze
    #
    #######################################################
    def WallDistance_unit_V(self, angle, i, j):

        # coordinates of extremities of wall
        ax = i * Maze.cell_width
        ay = j * Maze.cell_width
        bx = ax
        by = ay + Maze.cell_width
        
        # distance to top segment
        sax = bx - Maze.wall_width/2
        say = by 
        sbx = bx + Maze.wall_width/2
        sby = by 
        [dmin, tx, ty] = Distance2HSegment(self.current_pos_x, self.current_pos_y, angle, sax, say, sbx, sby)
        # distance to bottom segment
        sax = ax - Maze.wall_width/2
        say = ay 
        sbx = ax + Maze.wall_width/2
        sby = ay 
        [d, ttx, tty] = Distance2HSegment(self.current_pos_x, self.current_pos_y, angle, sax, say, sbx, sby)
        if d < dmin:
            dmin = d
            tx = ttx
            ty = tty
        # distance to left segment
        sax = ax - Maze.wall_width/2
        say = ay 
        sbx = bx - Maze.wall_width/2
        sby = by 
        [d, ttx, tty] = Distance2VSegment(self.current_pos_x, self.current_pos_y, angle, sax, say, sbx, sby)
        if d < dmin:
            dmin = d
            tx = ttx
            ty = tty
        # distance to right segment
        sax = ax + Maze.wall_width/2
        say = ay 
        sbx = bx + Maze.wall_width/2
        sby = by 
        [d, ttx, tty] = Distance2VSegment(self.current_pos_x, self.current_pos_y, angle, sax, say, sbx, sby)
        if d < dmin:
            dmin = d
            tx = ttx
            ty = tty
            
        return [dmin, tx, ty]


    #######################################################
    # WallDistance
    # return distance to nearest wall in maze
    #
    #######################################################
    def WallDistance(self, sensor_angle):
        
        dmin = math.inf
        txmin = math.inf
        tymin = math.inf
        imin = math.inf
        jmin = math.inf
        
        angle = self.current_heading + sensor_angle
        
        for i in range(0, self.maze.cellnb_x):
            for j in range(0, self.maze.cellnb_y):
                # distance to horizontal walls
                if self.maze.HWalls[self.maze.ij2n(i,j)] == True:
                    [d, tx, ty] = self.WallDistance_unit_H(angle, i, j)
                    if d < dmin:
                        dmin = d
                        imin = i
                        jmin = j
                        txmin = tx
                        tymin = ty
                # distance to vertical walls
                if self.maze.VWalls[self.maze.ij2n(i,j)] == True:
                    [d, tx, ty] = self.WallDistance_unit_V(angle, i, j)
                    if d < dmin:
                        dmin = d
                        imin = i
                        jmin = j
                        txmin = tx
                        tymin = ty
        # distance to horizontal walls on last line
        for i in range(0, self.maze.cellnb_x):
            j = self.maze.cellnb_y
            [d, tx, ty] = self.WallDistance_unit_H(angle, i, j)
            if d < dmin:
                dmin = d
                imin = i
                jmin = j
                txmin = tx
                tymin = ty
        # distance to vertical walls on last column
        for j in range(0, self.maze.cellnb_y):
            i = self.maze.cellnb_x
            [d, tx, ty] = self.WallDistance_unit_V(angle, i, j)
            if d < dmin:
                dmin = d
                imin = i
                jmin = j
                txmin = tx
                tymin = ty
        
        return [dmin, txmin, tymin, imin, jmin]
        
