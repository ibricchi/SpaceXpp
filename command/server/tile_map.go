/*
   Written by Bradley Stanley-Clamp (bradley.stanley-clamp19@imperial.ac.uk) and Nicholas Pfaff (nicholas.pfaff19@imperial.ac.uk), 2021 - SpaceX++ EEE/EIE 2nd year group project, Imperial College London
*/

package server

type tileMap struct {
	Rows  int   `json:"rows"`   // number of rows
	Cols  int   `json:"cols"`   // number of columns
	Tiles []int `json:"layout"` // matrix stored as slice
}

type mapDB struct {
	Rows          int                `json:"rows"`   // number of rows
	Cols          int                `json:"cols"`   // number of columns
	Tiles         []int              `json:"layout"` // matrix stored as slice
	RoverIndx     int                `json:"roverIndx"`
	RoverRotation int                `json:"roverRotation"`
	Instructions  []driveInstruction `json:"driveinstructions"`
}

func (m *tileMap) getTile(row int, col int) int {
	return m.Tiles[row*m.Cols+col]
}

type balls struct {
	red    bool
	yellow bool
	violet bool
	blue   bool
	teal   bool
}
