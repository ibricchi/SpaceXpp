package server

type tileMap struct {
	cols  int   // number of columns
	rows  int   // number of rows
	tiles []int // matrix stored as slice
}

func (m *tileMap) getTile(row int, col int) int {
	return m.tiles[row*m.cols+col]
}
