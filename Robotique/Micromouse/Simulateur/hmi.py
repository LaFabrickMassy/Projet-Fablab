from tkinter import *
from tkinter import messagebox
import numpy as np
from maze import *
from robot import *

###########################################################
#
###########################################################
class HMI():

    xshift = 2
    yshift = 2
    update_period = 500 # temporisation between simulation updates, in ms
    
    #######################################################
    def __init__(self, cellnb_x, cellnb_y):
    
        self.maze = Maze(cellnb_x, cellnb_y)
        self.robot = Robot(self.maze)
        
        self.tkapp = Tk()
        self.widget_robot = []
        self.widget_sensors = [] 
        
        self.saved = False
        self.status = ""

        self.InitTKWindow()
        
        ##### Define menu
        self.tkmenu = Menu(self.tkapp)
        # File menu
        self.tkmenu_file = Menu(self.tkmenu)
        self.tkmenu.add_cascade(label="File", menu = self.tkmenu_file)
        self.tkmenu_file.add_command(label = "Export", command = self.Menu_File_Export)
        self.tkmenu_file.add_command(label = "Import", command = self.Menu_File_Import)
        self.tkmenu_file.add_separator()
        self.tkmenu_file.add_command(label = "Exit", command = self.tkapp.destroy)
        # Maze menu
        self.tkmenu_maze = Menu(self.tkmenu)
        self.tkmenu.add_cascade(label="Maze", menu = self.tkmenu_maze)
        self.tkmenu_maze.add_command(label = "New (n)", command = self.NewMaze)
        self.tkmenu_maze.add_command(label = "Change size", command = self.ChangeSize)
        self.tkmenu_maze.add_separator()
        self.tkmenu_maze.add_command(label = "Solve (s)", command = self.Solve)
        # Robot menu
        self.tkmenu_robot = Menu(self.tkmenu)
        self.tkmenu.add_cascade(label="Robot", menu = self.tkmenu_robot)
        self.tkmenu_robot.add_command(label = "Explore (e)", command = self.RobotExplore)
        self.tkmenu_robot.add_command(label = "Run (r)", command = self.RobotRun)
        
        # show menu
        self.tkapp.config(menu=self.tkmenu)

        self.tkapp.bind('<Key>', self.KeyPressed)
        
    #######################################################
    def InitTKWindow(self):

        screen_width = self.tkapp.winfo_screenwidth()
        screen_height = self.tkapp.winfo_screenheight()
        
        screen_ratio = screen_width/screen_height
        maze_ratio = self.maze.cellnb_x/self.maze.cellnb_y
        if maze_ratio < screen_ratio:
            window_height = screen_height*0.8
            self.px_per_mm = window_height/(Maze.cell_width*self.maze.cellnb_y + Maze.wall_width)
            self.cell_pxsize = int((window_height-Maze.wall_width)/self.maze.cellnb_y)
        else:
            window_width = screen_width*0.8
            self.px_per_mm = window_width/(Maze.cell_width*self.maze.cellnb_x + Maze.wall_width)
            self.cell_pxsize = int((window_width-Maze.wall_width)/self.maze.cellnb_x)
        
        self.draw_width = int(self.px_per_mm * (self.maze.cellnb_x * Maze.cell_width + 2*Maze.wall_width)) + HMI.xshift -1
        self.draw_height = int(self.px_per_mm * (self.maze.cellnb_y * Maze.cell_width + 2*Maze.wall_width)) + HMI.yshift -1

        self.tkapp.geometry("{}x{}".format(self.draw_width, self.draw_height))

        self.canvas = Canvas(self.tkapp, width=self.draw_width, height=self.draw_height, background="white")
        self.canvas.place(x=0, y=0)
        
        self.SetWindowTitle()
    
    #######################################################
    def ChangeSize(self):

        self.ChangeSize_Open()
        """
        done = self.ChangeSize_End()
        while not done:
            self.ChangeSize_End()
        """
        
    #######################################################
    def ChangeSize_Open(self):
    
        self.change_size_window = Toplevel(self.tkapp)
        self.change_size_window.title("Change size")
        x_label = Label(self.change_size_window, text="Size x:").grid(row=0, column=0)
        y_label = Label(self.change_size_window, text="Size y:").grid(row=1, column=0)
        self.x_val_widget = Entry(self.change_size_window)
        self.x_val_widget.grid(row=0, column=1)
        self.y_val_widget = Entry(self.change_size_window)
        self.y_val_widget.grid(row=1, column=1)
        bok = Button(self.change_size_window, text = "OK", bd=5, command=self.ChangeSize_End)
        bok.grid(row=2, column=0)
        bcancel = Button(self.change_size_window, text = "Cancel", command=self.ChangeSize_Close)
        bcancel.grid(row=2, column=1)
        self.change_size_window.bind("<Return>", self.ChangeSize_End_Kbd)
        
    #######################################################
    def ChangeSize_End_Kbd(self, event):
        self.ChangeSize_End()
        
    #######################################################
    def ChangeSize_End(self):
    
        status = False
        
        x_val_str = self.x_val_widget.get()
        y_val_str = self.y_val_widget.get()
        
        if x_val_str == "" or y_val_str == "":
            status = False
        elif x_val_str.isnumeric() and y_val_str.isnumeric():
            Maze_cellnb_x = int(x_val_str)
            Maze_cellnb_y = int(y_val_str)
            self.maze = Maze(Maze_cellnb_x, Maze_cellnb_y)
            self.change_size_window.destroy()
            self.canvas.destroy()
            self.InitTKWindow()
            status = True
        else:
            messagebox.showwarning("Warning", "Enter only numbers !")
            status = False
        
        return status
            
    #######################################################
    def ChangeSize_Close(self):
    
        self.change_size_window.destroy()

    #######################################################
    def NewMaze(self):
    
        self.status = "Generating..."
        self.SetWindowTitle()
        self.maze.CleanUp()
        wall_density = self.maze.GenerateRandom_Init()
        self.DrawMaze()
        while wall_density < 1:
            wall_density = self.maze.GenerateRandom_AddWall()
            [i,j,f]=self.maze.last_wall
            if f: # last wall was horizontal
                self.DrawHWall(i, j)
            else:
                self.DrawVWall(i,j)
            self.SetWindowTitle()
            self.tkapp.update()
        self.robot=Robot(self.maze)
        self.DrawMaze()                        
        self.status = ""
        self.SetWindowTitle()
    
    #######################################################
    def SetWindowTitle(self):
    
        title = self.maze.name
        if not self.saved:
            title = title + "*"
        title = title + " " + str(self.maze.cellnb_x) + "x" + str(self.maze.cellnb_y)
        if self.maze.wall_density > 0:
          title = title + " ({}%)".format(int(100*self.maze.wall_density))
        title = title + " " + self.status
        self.tkapp.title(title)
        
    #######################################################
    # DrawLinePx
    # Draw a simple line in the window. 
    # Parameters are pixel coordinates
    #######################################################
    def DrawLinePx(self, ax, ay, bx, by, color="#000000"):
        self.canvas.create_line(ax, self.draw_height-ay, bx, self.draw_height-by, fill=color)

    #######################################################
    # DrawRectPx
    # Draw a simple line in the window. 
    # Parameters are pixel coordinates
    #######################################################
    def DrawRectPx(self, ax, ay, bx, by, color="#000000"):
        self.canvas.create_rectangle(ax, self.draw_height-ay, bx, self.draw_height-by, fill=color)
        
    #######################################################
    # DrawLine
    # Draw a line between two points. 
    #######################################################
    def DrawLine(self, ax, ay, bx, by, color="#000000"):

        ax = int(self.px_per_mm * ax) + HMI.xshift
        ay = int(self.px_per_mm * ay) + HMI.yshift
        bx = int(self.px_per_mm * bx) + HMI.xshift
        by = int(self.px_per_mm * by) + HMI.yshift

        self.canvas.create_line(ax, self.draw_height-ay, bx, self.draw_height-by, fill=color)

    #######################################################
    # DrawPathLine
    # Draw a line between center of cells. 
    # Used to show a path in maze.
    #######################################################
    def DrawPathLine(self, ai, aj, bi, bj, color="purple"):

        ax = int(self.px_per_mm * (ai * Maze.cell_width + Maze.cell_width/2)) + HMI.xshift
        ay = int(self.px_per_mm * (aj * Maze.cell_width + Maze.cell_width/2)) + HMI.yshift
        bx = int(self.px_per_mm * (bi * Maze.cell_width + Maze.cell_width/2)) + HMI.xshift
        by = int(self.px_per_mm * (bj * Maze.cell_width + Maze.cell_width/2)) + HMI.yshift

        self.canvas.create_line(ax, self.draw_height-ay, bx, self.draw_height-by, fill=color, dash=(3,1), width=4)

    #######################################################
    def DrawHWall(self, i, j, color="#000000"):
        
        ax = int(self.px_per_mm * (    i * Maze.cell_width)) + HMI.xshift
        ay = int(self.px_per_mm * (    j * Maze.cell_width - Maze.wall_width/2))  + HMI.yshift
        bx = int(self.px_per_mm * ((i+1) * Maze.cell_width)) + HMI.xshift
        by = int(self.px_per_mm * (    j * Maze.cell_width + Maze.wall_width/2)) + HMI.yshift

        self.canvas.create_rectangle(ax, self.draw_height-ay, bx, self.draw_height-by, fill=color)

    #######################################################
    def DrawVWall(self, i, j, color="#000000"):
        
        ax = int(self.px_per_mm * (    i * Maze.cell_width - Maze.wall_width/2)) + HMI.xshift
        ay = int(self.px_per_mm * (    j * Maze.cell_width)) + HMI.yshift
        bx = int(self.px_per_mm * (    i * Maze.cell_width + Maze.wall_width/2)) + HMI.xshift
        by = int(self.px_per_mm * ((j+1) * Maze.cell_width)) + HMI.yshift

        self.canvas.create_rectangle(ax, self.draw_height-ay, bx, self.draw_height-by, fill=color)

    #######################################################
    def DrawCell(self, i, j, color = "#cccccc"):

        ax = self.px_per_mm *(    i * Maze.cell_width + Maze.wall_width) + HMI.xshift
        ay = self.px_per_mm *(    j * Maze.cell_width + Maze.wall_width) + HMI.yshift
        bx = self.px_per_mm *((i+1) * Maze.cell_width - Maze.wall_width) + HMI.xshift
        by = self.px_per_mm *((j+1) * Maze.cell_width - Maze.wall_width) + HMI.yshift

        self.canvas.create_rectangle(ax, self.draw_height-ay, bx, self.draw_height-by, fill=color, outline=color)
        
    #######################################################
    def DrawMaze(self):

        # draw maze ground
        ax = 0
        ay = 0
        bx = self.px_per_mm * (self.maze.cellnb_x*Maze.cell_width + Maze.wall_width)
        by = self.px_per_mm * (self.maze.cellnb_y*Maze.cell_width + Maze.wall_width)
        self.canvas.create_rectangle(ax, self.draw_height-ay, bx, self.draw_height-by, fill="#cccccc", outline="#cccccc")

        # draw start and end cell, only if they exist
        if self.maze.end_cell:
            self.DrawCell(0,0, color = "green")
            self.DrawCell(self.maze.end_cell[0], self.maze.end_cell[1], color = "red")

        # draw interior walls
        for i in range(0, self.maze.cellnb_x):
            for j in range(0, self.maze.cellnb_y):
                if (i > 0) or (j > 0): # only interior walls
                    if self.maze.HWalls[self.maze.ij2n(i,j)] == True:
                        self.DrawHWall(i, j)
                    if self.maze.VWalls[self.maze.ij2n(i,j)] == True:
                        self.DrawVWall(i, j)
        # draw border walls
        for i in range(0, self.maze.cellnb_x):
            self.DrawHWall(i, 0)
            self.DrawHWall(i, self.maze.cellnb_y)
        for j in range(0, self.maze.cellnb_y):
            self.DrawVWall(0, j)
            self.DrawVWall(self.maze.cellnb_x, j)
            
        # draw robot
        self.DrawRobot()
            
    #######################################################
    def Real2CanvasCoordinates(self, rx, ry):
        x = self.px_per_mm * rx + HMI.xshift
        y = self.px_per_mm * ry + HMI.yshift
        return (x, y)
        
    #######################################################
    def DrawRobot(self):
        
        # delete existing robot drawings
        for w in self.widget_robot:
            self.canvas.delete(w)
        for w in self.widget_sensors:
            if w:
                self.canvas.delete(w)
        self.widget_robot = []
        self.widget_sensors = []
        
        self.robot.DistanceSensors()
        
        # draw robot body
        #print(f"Heading = {self.robot.current_heading}")
        (x, y) = self.Real2CanvasCoordinates(self.robot.current_pos_x, self.robot.current_pos_y)
        r = Robot.wheels_distance/2 * self.px_per_mm
         
        w = self.canvas.create_oval(x-r, self.draw_height - (y-r), x+r, self.draw_height - (y+r), outline="black", fill="black")
        self.widget_robot.append(w)
        
        # draw sensors view
        for sensor_data in self.robot.sensors_data:
            if sensor_data:
                [d, tx, ty] = sensor_data[:3]
                (tpx, tpy) = self.Real2CanvasCoordinates(tx, ty)
                w = self.canvas.create_line(x, self.draw_height-y, tpx, self.draw_height-tpy, fill="green")
                self.widget_sensors.append(w)
            
    #######################################################
    def Solve(self):
    
        path = self.maze.ShortestPath((0,0), self.maze.end_cell)
        for i in range(len(path)-1):
            (ai, aj) = path[i]
            (bi, bj) = path[i+1]
            self.DrawPathLine(ai, aj, bi, bj)
            
        self.status = f"solved in {len(path)-1} steps"
        self.SetWindowTitle()

    #######################################################
    # RobotExplore: launch exploration
    def RobotExplore(self):

        # initialise exploration
        self.robot.ExploreInit()
        time.sleep(Robot.simu_time_step)
        
        # main loop
        while self.robot.mode == Robot.MODE_EXPLORE:
            self.robot.ExploreUpdate()
            self.DrawRobot()
            self.tkapp.update()
            #self.tkapp.after(HMI.update_period, self.RobotUpdate)
            time.sleep(Robot.simu_time_step)
        
    #######################################################
    # RobotRun: 
    def RobotRun(self):
        
        # initialisations
        path = self.maze.ShortestPath((0,0), self.maze.end_cell)
        moves = self.robot.Path2Moves(path)
        self.robot.StartCellCenter()
        self.DrawRobot()
        
        # main loop
        steps = 100
        for move in moves[:-1]: # last move is stop, don't use it
            if move == Robot.MVT_AHEAD:
                dx = self.maze.cell_width/steps * math.cos(self.robot.current_heading)
                dy = self.maze.cell_width/steps * math.sin(self.robot.current_heading)
                dh = 0
            elif move == Robot.MVT_TURNR:
                dx = 0
                dy = 0
                dh = -math.pi/2/steps
            elif move == Robot.MVT_TURNL:
                dx = 0
                dy = 0
                dh = math.pi/2/steps
            for i in range(steps):
                self.robot.current_pos_x += dx
                self.robot.current_pos_y += dy
                self.robot.current_heading += dh
                self.DrawRobot()
                self.tkapp.update()
                #time.sleep(0.01)
                    
                
            
        
    #######################################################
    def RobotRunUpdate(self):
        if self.robot.CurrentStatus() == "run":
            self.robot.RunUpdate()
            self.DrawRobot()
            self.tkapp.after(HMI.update_period, self.RobotUpdate)
    
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
    def KeyPressed(self, event):
        key = event.char
        if key == 'q':
            self.tkapp.quit()
        elif key == 'N': # to test step by step maze generation
            self.NewMaze_Init()
        elif key == 'C': # to test step by step maze generation
            self.NewMaze_Continue()
        elif key == 'n':
            self.NewMaze()
        elif key == 's':
            self.Solve()
        elif key == 't':
            self.Test()
        elif key == 'r':
            self.RobotRun()
        elif key == 'e':
            self.RobotExplore()
        else:
            pass
            #print(f"<{key}>")
            #print(int(key))
        
    #######################################################
    # Test functions
    #######################################################
   
    #######################################################
    def TestCreateUnit(self, i, j, rot):
        if rot == 0:
            self.maze.AddHWall(i  , j  )
            self.maze.AddVWall(i+1, j-1)
            self.maze.AddVWall(i+1, j-2)
            self.maze.AddHWall(i+1, j-2)
            self.maze.AddHWall(i+2, j-2)
            self.maze.AddVWall(i+3, j-2)
            self.maze.AddHWall(i+2, j-1)
            self.maze.AddVWall(i+2, j-1)
            self.maze.AddHWall(i+2, j  )
        if rot == 1:
            self.maze.AddVWall(i  , j  )
            self.maze.AddHWall(i  , j+1)
            self.maze.AddHWall(i+1, j+1)
            self.maze.AddVWall(i+2, j+1)
            self.maze.AddVWall(i+2, j+2)
            self.maze.AddHWall(i+1, j+3)
            self.maze.AddVWall(i+1, j+2)
            self.maze.AddHWall(i  , j+2)
            self.maze.AddVWall(i  , j+2)
        if rot == 2:
            self.maze.AddHWall(i-1, j  )
            self.maze.AddVWall(i-1, j  )
            self.maze.AddVWall(i-1, j+1)
            self.maze.AddHWall(i-2, j+2)
            self.maze.AddHWall(i-3, j+2)
            self.maze.AddVWall(i-3, j+1)
            self.maze.AddHWall(i-3, j+1)
            self.maze.AddVWall(i-2, j  )
            self.maze.AddHWall(i-3, j  )
        if rot == 3:
            self.maze.AddVWall(i  , j-1)
            self.maze.AddHWall(i-1, j-1)
            self.maze.AddHWall(i-2, j-1)
            self.maze.AddVWall(i-2, j-2)
            self.maze.AddVWall(i-2, j-3)
            self.maze.AddHWall(i-2, j-3)
            self.maze.AddVWall(i-1, j-3)
            self.maze.AddHWall(i-1, j-2)
            self.maze.AddVWall(i  , j-3)
        
    #######################################################
    def Test_Sensor(self):
        # create a 12x12 maze
        self.status = "Test"
        self.SetWindowTitle()
        Maze_cellnb_x = 12
        Maze_cellnb_y = 12
        self.maze = Maze(Maze_cellnb_x, Maze_cellnb_y)
        self.robot = Robot(self.maze)
        self.canvas.destroy()
        self.InitTKWindow()
        self.maze.CleanUp()
        
        # Create test walls
        self.TestCreateUnit(3,3,0)
        self.TestCreateUnit(6,3,0)
        self.TestCreateUnit(9,3,1)
        self.TestCreateUnit(9,6,1)
        self.TestCreateUnit(9,9,2)
        self.TestCreateUnit(6,9,2)
        self.TestCreateUnit(3,9,3)
        self.TestCreateUnit(3,6,3)
        """

        self.maze.AddVWall(6  ,6)
        self.maze.AddHWall(5  ,6)
        """
        
        self.robot=Robot(self.maze)
        self.robot.current_pos_x = 5.5*Maze.cell_width
        self.robot.current_pos_y = 6.5*Maze.cell_width
        
        print(f"Robot position : {self.robot.current_pos_x},{self.robot.current_pos_y}")
        self.DrawMaze()
        
        """
        [d,tx,ty] = self.robot.WallDistance_unit_V(0, 6,6)
        print([d, tx, ty])
        [d,tx,ty] = self.robot.WallDistance_unit_H(-math.pi/2, 5,6)
        print([d, tx, ty])
        """
        
        print("Test_Sensor : launching tests")
        nbsteps = 8
        for angle in [math.pi/4]: #np.linspace(0, 2*math.pi, nbsteps, False):
            print(f"Test angle = {math.degrees(angle)}")
            #[d, tx, ty, imin, ymin] = self.robot.WallDistance(angle)
            self.robot.current_heading = angle
            self.robot.DistanceSensors()
            [d, tx, ty] = self.robot.sensors_data[0][:3]
            self.DrawLine(self.robot.current_pos_x, self.robot.current_pos_y, tx, ty, color = "red")

    #######################################################
    def Test(self):
    
        self.Test_Sensor()
        """
        # create a 12x12 maze
        self.status = "Test"
        self.robot.current_heading = 0
        self.DrawRobot()
        self.tkapp.update()
        time.sleep(1)
        self.robot.current_heading = math.radians(30)
        self.DrawRobot()
        self.tkapp.update()
        time.sleep(1)
        self.robot.current_heading = math.radians(60)
        self.DrawRobot()
        self.tkapp.update()
        time.sleep(1)
        self.robot.current_heading = math.radians(90)
        self.DrawRobot()
        self.tkapp.update()
        time.sleep(1)
        self.robot.current_heading = math.radians(120)
        self.DrawRobot()
        self.tkapp.update()
        time.sleep(1)
        self.robot.current_heading = math.radians(150)
        self.DrawRobot()
        self.tkapp.update()
        time.sleep(1)
        self.robot.current_heading = math.radians(180)
        self.DrawRobot()
        self.tkapp.update()
        time.sleep(1)
        
        """
        
        

        