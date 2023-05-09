#include <Arduino.h>
#include "maze.h"

//*****************************************************************************
//
// Maze initialisation
//
//*****************************************************************************
Maze::Maze() {
    Reset();
}

//*****************************************************************************
//
// reset internal data
//
//*****************************************************************************
void Maze::Reset() {
    int i,j,n;

    orientation = MAZE_ORIENTATION_UNKNOWN;

    // initialise walls and cells
    for (i=0;i<MAZE_X_SIZE+1;i++) {
        for (j=0;j<MAZE_Y_SIZE+1;j++) {
            n = WallID_ij2n(i,j);
            hwalls[n] = MAZE_WALL_UNKNOWN;
            vwalls[n] = MAZE_WALL_UNKNOWN;
        }
    }
    for (i=0;i<MAZE_X_SIZE;i++) {
        for (j=0;j<MAZE_Y_SIZE;j++) {
            n = CellID_ij2n(i,j);
            cells[n] = MAZE_CELL_NO_INFO;
        }
    }

}

//*****************************************************************************
//
// convert wall id (x,y) -> n 
//
//*****************************************************************************
int Maze::WallID_ij2n(int i, int j) {
    return i+j*(MAZE_X_SIZE+1);
}

//*****************************************************************************
//
// convert cell id (x,y) -> n 
//
//*****************************************************************************
int Maze::CellID_ij2n(int i, int j) {
    return i+j*(MAZE_X_SIZE);
}

//*****************************************************************************
//
// 
//
//*****************************************************************************
String Maze::Status2Json() {
    int i, j;

    String s;

    s ="{";
    s +="size:[\'";
    s +=String(MAZE_X_SIZE);
    s +="\",\'";
    s +=String(MAZE_Y_SIZE);
    s +="\']";
    s += ",orientation:"+String(orientation);
    s +=",hwalls:[";
    for (i=0;i<(MAZE_X_SIZE+1)*(MAZE_Y_SIZE+1)-1;i++)
        s += "\""+ String(hwalls[i])+ "\",";
    s += "\""+ String(hwalls[i])+ "\"]";
    s +=",vwalls:[";
    for (i=0;i<(MAZE_X_SIZE+1)*(MAZE_Y_SIZE+1)-1;i++)
        s += "\""+ String(vwalls[i])+ "\",";
    s += "\""+ String(vwalls[i])+ "\"]";
    s +=",cells:[";
    for (i=0;i<(MAZE_X_SIZE)*(MAZE_Y_SIZE)-1;i++)
        s += "\""+ String(cells[i])+ "\",";
    s += "\""+ String(cells[i])+ "\"]";
    s += "}";

    return s;
}


