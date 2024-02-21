import math
import time
from maze import *
from sensors import *

# heading PID parameters
headPID_kp = 1
headPID_ki = 0
headPID_kd = 0


###########################################################
#
###########################################################
class Robot:

    simu_time_step = 0.1 # time step in seconds
    wheels_distance = 100 # distance between wheels, in mm
    
    distance_sensors_dir = [-75.*math.pi/180., 0, 75.*math.pi/180.] # [-math.pi/4, 0, math.pi/4]

    # movements : 
    MVT_STOP = 0 # stop
    MVT_AHEAD = 1 # ahead
    MVT_TURNL = 2 # turn left
    MVT_TURNR = 3 # turn right
    MVT_BACK = 4
    MVT_TO_TURNL = 5 # continue to center ov cell then turn left
    MVT_TO_TURNR = 6 # continue to center ov cell then turn left
    MVT_TO_BACK = 7

    # directions:
    DIR_RIGHT = 1
    DIR_UP = 2
    DIR_LEFT = 3
    DIR_DWN = 4
    
    # Status
    MODE_NONE = 0
    MODE_EXPLORE = 1
    MODE_SOLVE = 2
    MODE_RETURN_BACK = 3
    
    # Errors
    START_HEADING_ERROR = math.pi/24
    START_LATERAL_ERROR = wheels_distance*0.1
    
    # PID parameters
    Kp = 5
    Ki = 1
    Kd = 0

    # Gaussian parameters for initial positions
    pos_x_sigma = 10
    pos_x_sigma2 = pos_x_sigma*pos_x_sigma
    pos_y_sigma = 5
    pos_y_sigma2 = pos_y_sigma*pos_y_sigma
    pos_h_sigma = math.radians(5)
    pos_h_sigma2 = pos_h_sigma*pos_h_sigma
    # Gaussian parameters for sensors
    dist_sensorL_sigma = 3
    dist_sensorR_sigma = 3
    dist_sensorF_sigma = 3
    wheel_sensorL_sigma = 1
    wheel_sensorR_sigma = 1

    #######################################################
    def __init__(self, maze):
    
        self.maze = maze
        
        self.mode = Robot.MODE_NONE
        
        # sensor data
        self.sensor = [None] * len(Robot.distance_sensors_dir)

        self.current_pos_x = 0
        self.current_pos_y = 0
        self.current_heading = 0
        self.current_wspeedL = 0
        self.current_wspeedR = 0
        self.sensors_data = [None] * len(Robot.distance_sensors_dir)
        
        self.wall_side_dist = (Maze.cell_width - Maze.wall_width)/2*math.sqrt(2)
        self.wall_side_dmin = Robot.wheels_distance/2*math.sqrt(2)
        self.wall_side_dmax = 2*(Maze.cell_width - Maze.wall_width)/2*math.sqrt(2)
        self.wall_front_dist = Maze.cell_width/2
        self.wall_front_dmin = Robot.wheels_distance/2
        self.wall_front_dmax = Maze.cell_width
        
        self.current_state = Robot.MVT_STOP

        self.rmotor_speed = 0.
        self.lmotor_speed = 0.
        self.headPID_error = 0
        self.headPID_olderror = 0
        
        self.StartCellExplore()
    
    #######################################################
    #
    #######################################################
    def StartCellCenter(self):
        if self.maze.generated == False or self.maze.start_H:
            # 1st cell output on the right
            self.current_heading = 0 
            self.current_pos_x = Robot.wheels_distance/2 + Maze.wall_width  # (Maze.cell_width - Maze.wall_width)/2
            self.current_pos_y = (Maze.cell_width - Maze.wall_width)/2 
        else:
            # 1st cell output on the top
<<<<<<< Updated upstream
            self.current_heading = math.pi/2
            self.current_pos_x = (Maze.cell_width - Maze.wall_width)/2
            self.current_pos_y = (Maze.cell_width - Maze.wall_width)/2 #Robot.wheels_distance/2 + Maze.wall_width
        self.current_wspeedL = 0
        self.current_wspeedR = 0
=======
            self.current_heading = math.pi/2 
            self.current_pos_x = (Maze.cell_width - Maze.wall_width)/2 
            self.current_pos_y = Robot.wheels_distance/2 + Maze.wall_width
>>>>>>> Stashed changes

    #######################################################
    #
    #######################################################
    def StartCellExplore(self):
        if self.maze.generated == False or self.maze.start_H:
            # 1st cell output on the right
            self.current_heading = 0 + random.gauss(0, self.pos_h_sigma)
            self.current_pos_x = Robot.wheels_distance/2 + Maze.wall_width + random.gauss(0, self.pos_y_sigma) # (Maze.cell_width - Maze.wall_width)/2
            self.current_pos_y = (Maze.cell_width - Maze.wall_width)/2 + random.gauss(0, self.pos_x_sigma)
        else:
            # 1st cell output on the top
            self.current_heading = math.pi/2 + random.gauss(0, self.pos_h_sigma)
            self.current_pos_x = (Maze.cell_width - Maze.wall_width)/2 + random.gauss(0, self.pos_x_sigma)
            self.current_pos_y = Robot.wheels_distance/2 + Maze.wall_width+ random.gauss(0, self.pos_y_sigma)
            
        print("StartCellExplore {}".format((math.degrees(self.current_heading), self.current_pos_x, self.current_pos_y)))

    #######################################################
    # 
    #######################################################
    def RunStepMotorSpeed(self, lspeed, rspeed):
        """
        Compute new position from motors speed
        
        """
        dL = lspeed*self.simu_time_step
        dR = rspeed*self.simu_time_step
        
        #print("RunStepMotorSpeed {:.1f},{:.1f}".format(dL, dR))
        W = self.wheels_distance
        
        if dL > 0:
            if dR > 0:
                if dL > dR: # case 1
                    #print(f"  # case 1, {dL},{dR}")
                
                    q = dL/dR
                    rR = W/(q-1.)
                    r = rR + W/2
                    alpha = -dR / rR
                    ch = math.cos(self.gen_heading)
                    sh = math.sin(self.gen_heading)
                    ca = math.cos(alpha)
                    sa = math.sin(alpha)
                    
                    # O on right
                    mov_x = +r * (sh*(1.-ca) - ch*sa)
                    mov_y = -r * (ch*(1.-ca) + sh*sa)
                    
                elif dL == dR: # case 2
                    #print("  # case 2, {dL},{dR}")
                
                    alpha = 0.
                    r = dL
                    ch = math.cos(self.gen_heading)
                    sh = math.sin(self.gen_heading)

                    # O on center
                    mov_x = r*ch
                    mov_y = r*sh
                    
                else: # dL < dR, case 3
                    #print(f"  # case 3, {dL},{dR}")

                    q = dR/dL
                    rL = W/(q-1.)
                    r = rL+W/2.
                    alpha = dL / rL
                    ch = math.cos(self.gen_heading)
                    sh = math.sin(self.gen_heading)
                    ca = math.cos(alpha)
                    sa = math.sin(alpha)
                    
                    # O on left
                    mov_x = -r * (sh*(1.-ca) - ch*sa)
                    mov_y =  r * (ch*(1.-ca) + sh*sa)

            elif dR == 0: # case 4
                #print(f"  # case 4, {dL},{dR}")

                r = W/2.
                alpha = -dL/W
                ch = math.cos(self.gen_heading)
                sh = math.sin(self.gen_heading)
                ca = math.cos(alpha)
                sa = math.sin(alpha)
                
                # O on right
                mov_x = +r * (sh*(1.-ca) - ch*sa)
                mov_y = -r * (ch*(1.-ca) + sh*sa)

            else: # dR < 0
                if dL < -dR: # case 5
                    #print(f"  # case 5, {dL},{dR}")
            
                    q = -dL/dR
                    rR = W/(q+1.)
                    r = W/2.-rR
                    alpha = -dR / rR
                    ch = math.cos(self.gen_heading)
                    sh = math.sin(self.gen_heading)
                    ca = math.cos(alpha)
                    sa = math.sin(alpha)
                    
                    # O on right
                    mov_x = +r * (sh*(1.-ca) - ch*sa)
                    mov_y = -r * (ch*(1.-ca) + sh*sa)

                elif dL == -dR: # case 6
                    #print(f"  # case 6, {dL},{dR}")

                    r = 0.
                    alpha = -dL/(W/2.)

                    # O on center
                    mov_x = 0.
                    mov_y = 0.

                else: # dL > -dR, case 7
                    #print(f"  # case 7, {dL},{dR}")

                    q = -dR/dL
                    rL = W/(q+1.)
                    r = W/2.-rL
                    alpha = -dL / rL
                    ch = math.cos(self.gen_heading)
                    sh = math.sin(self.gen_heading)
                    ca = math.cos(alpha)
                    sa = math.sin(alpha)
                    
                    # O on left
                    mov_x = -r * (sh*(1.-ca) - ch*sa)
                    mov_y =  r * (ch*(1.-ca) + sh*sa)

        elif dL == 0:
            if dR > 0: # case 8
                #print(f"  # case 8, {dL},{dR}")

                r = W/2.
                alpha = dR/W
                ch = math.cos(self.gen_heading)
                sh = math.sin(self.gen_heading)
                ca = math.cos(alpha)
                sa = math.sin(alpha)
                
                # O on left
                mov_x = -r * (sh*(1.-ca) - ch*sa)
                mov_y =  r * (ch*(1.-ca) + sh*sa)

            elif dR == 0: # case 9
                #print(f"  # case 9, {dL},{dR}")

                r = 0.
                alpha = 0.
                mov_x = 0.
                mov_y = 0.

            else: # dR < 0, case 10
                #print(f"  # case 10, {dL},{dR}")

                r = W/2.
                alpha = dR/W
                
                ch = math.cos(self.gen_heading)
                sh = math.sin(self.gen_heading)
                ca = math.cos(alpha)
                sa = math.sin(alpha)
                
                # O on left
                mov_x = -r * (sh*(1.-ca) - ch*sa)
                mov_y =  r * (ch*(1.-ca) + sh*sa)

        else: # dL < 0
            if dR > 0:
                if -dL < dR: # case 11
                    #print(f"  # case 11, {dL},{dR}")

                    q = -dR/dL
                    rL = W/(q+1.)
                    r = W/2. - rL
                    alpha = -dL / rL
                    ch = math.cos(self.gen_heading)
                    sh = math.sin(self.gen_heading)
                    ca = math.cos(alpha)
                    sa = math.sin(alpha)
                    
                    # O on left
                    mov_x = -r * (sh*(1.-ca) - ch*sa)
                    mov_y =  r * (ch*(1.-ca) + sh*sa)

                elif -dL == dR: # case 12
                    #print(f"  # case 12, {dL},{dR}")

                    r = 0
                    alpha = -dL/(W/2.)
                    mov_x = 0.
                    mov_y = 0.

                else: # -dL > dR, case 13
                    #print(f"  # case 13, {dL},{dR}")

                    q = -dL/dR
                    rR = W/(q+1.)
                    r = W/2.-rR
                    alpha = -dR / rR
                    ch = math.cos(self.gen_heading)
                    sh = math.sin(self.gen_heading)
                    ca = math.cos(alpha)
                    sa = math.sin(alpha)
                    
                    # O on right
                    mov_x = +r * (sh*(1.-ca) - ch*sa)
                    mov_y = -r * (ch*(1.-ca) + sh*sa)

            elif dR == 0: # case 14
                #print(f"  # case 14, {dL},{dR}")

                r = W/2.
                alpha = -dL/W
                ch = math.cos(self.gen_heading)
                sh = math.sin(self.gen_heading)
                ca = math.cos(alpha)
                sa = math.sin(alpha)
                
                # O on right
                mov_x = +r * (sh*(1.-ca) - ch*sa)
                mov_y = -r * (ch*(1.-ca) + sh*sa)

            else: # dR < 0
                if dL < dR: # case 15
                    #print(f"  # case 15, {dL},{dR}")

                    q = dL/dR
                    rR = W/(q-1.)
                    r = W/2.+rR
                    alpha = -dR / rR
                    ch = math.cos(self.gen_heading)
                    sh = math.sin(self.gen_heading)
                    ca = math.cos(alpha)
                    sa = math.sin(alpha)
                    
                    # O on right
                    mov_x = +r * (sh*(1.-ca) - ch*sa)
                    mov_y = -r * (ch*(1.-ca) + sh*sa)

                if dL == dR: # case 16
                    #print(f"  # case 16, {dL},{dR}")

                    r = -dL
                    alpha = 0.
                    ch = math.cos(self.gen_heading)
                    sh = math.sin(self.gen_heading)
                    
                    mov_x = -r*ch
                    mov_y = -r*sh

                else: # dL > dR, case 17
                    #print(f"  # case 17, {dL},{dR}")

                    q = dR/dL
                    rL = W/(q-1.)
                    r = rL+W/2.
                    alpha = dL / rL
                    ch = math.cos(self.gen_heading)
                    sh = math.sin(self.gen_heading)
                    ca = math.cos(alpha)
                    sa = math.sin(alpha)
                    
                    # O on left
                    mov_x = -r * (sh*(1.-ca) - ch*sa)
                    mov_y =  r * (ch*(1.-ca) + sh*sa)

        self.gen_heading += alpha;
        self.gen_pos_x += mov_x;
        self.gen_pos_y += mov_y;

    #######################################################
    #
    #######################################################
    def RunStepAngle(self, speed, angle):
        
        distance = speed*self.simu_time_step
        
        new_heading = self.current_heading + angle
        
        dx = distance * math.cos(new_heading)
        dy = distance * math.sin(new_heading)
        
        self.current_pos_x += dx
        self.current_pos_y += dy
        self.current_heading = new_heading
        print("RunStepAngle ({},{})->({})".format(speed, angle, (self.current_pos_x, self.current_pos_y, self.current_heading)))
    
    #######################################################
    # RunStepCurvature
    # compute new position from speed and curvature
    # speed is set for external wheel. It has main impact for high curvature values.
    # curvature is > 0 when turning right (clockwise). 
    # curvature = 0 for straight forward, +/-math.inf for on place turn
    #######################################################
    def RunStepCurvature(self, speed, curvature):
    
        distance = speed*self.simu_time_step
        
        
        if curvature == 0:
            dx = distance * math.cos(current_heading)
            dy = distance * math.sin(current_heading)
            
            self.current_pos_x += dx
            self.current_pos_y += dy
            
            print("RunStep ahead: {}".format((dx, dy)))
        elif curvature == math.inf: # clockwise on place rotation, change only heading

            # radius of curve circle for external wheel
            radius_ext = Robot.wheels_distance

            # rotation angle
            angle = distance / radius_ext
            
            # new heading
            self.heading = self.heading - angle

            print("RunStep CW: {}".format(angle))
        elif curvature == -math.inf: # counterclockwise on place rotation, change only heading

            # radius of curve circle for external wheel
            radius_ext = Robot.wheels_distance

            # rotation angle
            angle = distance / radius_ext
            
            # new heading
            self.heading = self.heading + angle

            print("RunStep CCW: {}".format(angle))
        else: # general case, change heading and center position
        
            # radius or curve circle for center of robot
            radius = 1/curvature
            
            # radius of curve circle for external wheel
            radius_ext = radius + Robot.wheels_distance
            
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
            o_y = self.current_pos_y + radius * uorth_y
            
            # position of external wheel
            R_x = o_x - radius_ext*u_x
            R_y = o_y - radius_ext*u_y
            
            # rotation angle
            angle = distance / radius_ext
            
            if curvature > 0:
                # new heading
                self.current_heading = self.current_heading - angle
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
                self.current_heading = self.current_heading + angle
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

            print("RunStep Gen: {}->{}".format((speed, curvature), (self.current_pos_x, self.current_pos_y)))
            
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
        
        angle = sensor_angle
        
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
        
    #######################################################
    # DistanceSensors
    # return distance measured by sensors
    #
    #######################################################
    def DistanceSensors(self):
       
        self.sensors_data = []
        for a in Robot.distance_sensors_dir:
            angle = self.current_heading + a
            #print(f"DistanceSensors: heading {math.degrees(self.current_heading)}, a={math.degrees(a)}")
            #print(f"Angle {math.degrees(angle)}")
            [d, tx, ty, i, j] = self.WallDistance(angle)
            self.sensors_data.append([d, tx, ty, math.degrees(angle)])
        #print(f"DistanceSensors: sensors_data={self.sensors_data}")

    #######################################################
    # Movement
    # Compute dx, dy and dh from L and R speeds and dt
    #######################################################
    def Movement(self, l_speed, r_speed, dt):
        dL = l_speed * dt
        dR = r_speed * dt

        if dL > 0:
            if dR > 0:
                if dL > dR:
                    q = dL/dR
                    rR = W/(q-1.)
                    r = rR+W/2.
                    dh = -dR / rR;      
                    ch = math.cos(h)
                    sh = math.sin(h)
                    ca = math.cos(dh)
                    sa = math.sin(dh)
                    
                    # O on right
                    dx = -r * (sh*(1.-ca) - ch*sa)
                    dy = -r * (ch*(1.-ca) - sh*sa)
                elif dL == dR:
                    pass
                else:
                    pass
            elif dR == 0:
                if dL > dR:
                    pass
                elif dL == dR:
                    pass
                else:
                    pass
            else: # dR < 0
                if dL > dR:
                    pass
                elif dL == dR:
                    pass
                else:
                    pass
        elif dL == 0:
            if dR > 0:
                if dL > dR:
                    pass
                elif dL == dR:
                    pass
                else:
                    pass
            elif dR == 0:
                if dL > dR:
                    pass
                elif dL == dR:
                    pass
                else:
                    pass
            else: # dR < 0
                if dL > dR:
                    pass
                elif dL == dR:
                    pass
                else:
                    pass
        else: # dL <0
            if dR > 0:
                if dL > dR:
                    pass
                elif dL == dR:
                    pass
                else:
                    pass
            elif dR == 0:
                if dL > dR:
                    pass
                elif dL == dR:
                    pass
                else:
                    pass
            else: # dR < 0
                if dL > dR:
                    pass
                elif dL == dR:
                    pass
                else:
                    pass

    #######################################################
    # ExploreInit
    #######################################################
    def ExploreInit(self):

        # Set initial position
        self.StartCellExplore()

        # set flags        
        self.mode = Robot.MODE_EXPLORE
        self.mov  = Robot.MVT_AHEAD
        self.stabilised = False
        
        self.side_hole_found = False
        
        self.step_time = time.time()


    #######################################################
    # ExploreUpdate
    #######################################################
    def ExploreUpdate(self):
    
        # predict position from state


        # get sensors data
        self.DistanceSensors()
        [[ld, lx, ly, la], [cd, cy, cy, ca],[rd, rx, ry, ra]] = self.sensors_data
<<<<<<< Updated upstream

        # update prediction from sensors

        
        # check if walls around
        if ld > self.wall_side_dmax:
            # no wall on left
            wall_left = False
=======
        
        if not self.stabilised:
            # not stabilised, use PID
            headPID_error = ld-rd
            up = headPID_kp * headPID_error
            ui = headPID_ki * (headPID_error + self.headPID_olderror)
            ud = headPID_kd * (headPID_error - self.headPID_olderror)
            self.lmotor_speed += up + ui + ud
            self.rmotor_speed -= up + ui + ud
            self.RunStepMotorSpeed (self.lmotor_speed, self.rmotor_speed)
>>>>>>> Stashed changes
        else:
            pass


        
    #######################################################
    # Path2Moves
    #######################################################
    def Path2Moves(self, path):
        
        moves = []
        
        # set initial status : position and heading
        cur_i = 0
        cur_j = 0
        if self.maze.generated == False or self.maze.start_H:
            cur_dir = Robot.DIR_RIGHT # facing up
        else:
            cur_dir = Robot.DIR_UP # facing right
            
        for cell_ij in path:
            new_i = cell_ij[0]
            new_j = cell_ij[1]
            if new_i > cur_i: # new cell is on the right
                if cur_dir == Robot.DIR_RIGHT: 
                    # step ahead
                    moves.append(Robot.MVT_AHEAD)
                elif cur_dir == Robot.DIR_LEFT: # were going left, impossible but...
                    # turn right twice then ahead
                    moves.append(Robot.MVT_TURNR)
                    moves.append(Robot.MVT_TURNR)
                    moves.append(Robot.MVT_AHEAD)
                elif cur_dir == Robot.DIR_UP: # were going up
                    # turn right then ahead
                    moves.append(Robot.MVT_TURNR)
                    moves.append(Robot.MVT_AHEAD)
                elif cur_dir == Robot.DIR_DWN: # were going down
                    # turn left then ahead
                    moves.append(Robot.MVT_TURNL)
                    moves.append(Robot.MVT_AHEAD)
                else:
                    # unknown case
                    pass
                cur_dir = Robot.DIR_RIGHT
            elif new_i < cur_i: # new cell is on the left
                if cur_dir == Robot.DIR_LEFT: 
                    # step ahead
                    moves.append(Robot.MVT_AHEAD)
                elif cur_dir == Robot.DIR_RIGHT: # were going right, impossible but...
                    # turn right twice then ahead
                    moves.append(Robot.MVT_TURNR)
                    moves.append(Robot.MVT_TURNR)
                    moves.append(Robot.MVT_AHEAD)
                elif cur_dir == Robot.DIR_UP: # were going up
                    # turn left then ahead
                    moves.append(Robot.MVT_TURNL)
                    moves.append(Robot.MVT_AHEAD)
                elif cur_dir == Robot.DIR_DWN: # were going down
                    # turn right then ahead
                    moves.append(Robot.MVT_TURNR)
                    moves.append(Robot.MVT_AHEAD)
                else:
                    # unknown case
                    pass
                cur_dir = Robot.DIR_LEFT
            elif new_j > cur_j:  # new cell is up
                if cur_dir == Robot.DIR_UP: 
                    # step ahead
                    moves.append(Robot.MVT_AHEAD)
                elif cur_dir == Robot.DIR_DWN: # were going down, impossible but...
                    # turn right twice then ahead
                    moves.append(Robot.MVT_TURNR)
                    moves.append(Robot.MVT_TURNR)
                    moves.append(Robot.MVT_AHEAD)
                elif cur_dir == Robot.DIR_LEFT: # were going left
                    # turn right then ahead
                    moves.append(Robot.MVT_TURNR)
                    moves.append(Robot.MVT_AHEAD)
                elif cur_dir == Robot.DIR_RIGHT: # were going right
                    # turn left then ahead
                    moves.append(Robot.MVT_TURNL)
                    moves.append(Robot.MVT_AHEAD)
                else:
                    # unknown case
                    pass
                cur_dir = Robot.DIR_UP
            elif new_j < cur_j:  # new cell is down
                if cur_dir == Robot.DIR_DWN: 
                    # step ahead
                    moves.append(Robot.MVT_AHEAD)
                elif cur_dir == Robot.DIR_UP: # were going up, impossible but...
                    # turn right twice then ahead
                    moves.append(Robot.MVT_TURNR)
                    moves.append(Robot.MVT_TURNR)
                    moves.append(Robot.MVT_AHEAD)
                elif cur_dir == Robot.DIR_RIGHT: # were going right
                    # turn right then ahead
                    moves.append(Robot.MVT_TURNR)
                    moves.append(Robot.MVT_AHEAD)
                elif cur_dir == Robot.DIR_LEFT: # were going left
                    # turn left then ahead
                    moves.append(Robot.MVT_TURNL)
                    moves.append(Robot.MVT_AHEAD)
                else:
                    # unknown case
                    pass
                cur_dir = Robot.DIR_DWN
            else:
                # unknown case
                pass
            cur_i = new_i
            cur_j = new_j
        
        # last movement : stop
        moves.append(Robot.MVT_STOP)
        
        return moves