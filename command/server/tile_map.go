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
