package server

type tileMap struct {
	rows  int   // number of rows
	cols  int   // number of columns
	tiles []int // matrix stored as slice
}

func (m *tileMap) getTile(row int, col int) int {
	return m.tiles[row*m.cols+col]
}
