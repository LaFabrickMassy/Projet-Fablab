#ifndef _MAZE_H_
#define _MAZE_H_

// Maze parameters
#define MAZE_WALL_LENGTH 204
#define MAZE_WALL_THICKNESS 9
#define MAZE_X_SIZE 9
#define MAZE_Y_SIZE 9

// Maze orientation
#define MAZE_ORIENTATION_UNKNOWN 0
#define MAZE_ORIENTATION_NORTH 1
#define MAZE_ORIENTATION_WEST 2

// information about a wall
#define MAZE_WALL_NONE 0
#define MAZE_WALL_UP 1
#define MAZE_WALL_UNKNOWN 99

// Information about a cell during exploration
#define MAZE_CELL_NO_INFO 0
#define MAZE_CELL_PARTIAL_INFO 1
#define MAZE_CELL_FULL_INFO 2


class Maze {
    public:
        Maze();
        void Reset();
        int WallID_ij2n(int i, int j);
        int CellID_ij2n(int i, int j);
        String Status2Json();

    private:
        // True when maze is explored
        int explored;
        // Maze representation data
        int orientation;
        int hwalls[(MAZE_X_SIZE+1)*(MAZE_Y_SIZE+1)];
        int vwalls[(MAZE_X_SIZE+1)*(MAZE_Y_SIZE+1)];
        int cells[MAZE_X_SIZE*MAZE_Y_SIZE];
};




#endif