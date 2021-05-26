package server

import (
	"reflect"
	"testing"
)

func TestGetShortedPathFromStartToDestination(t *testing.T) {
	type test struct {
		startRow       int
		startCol       int
		destinationRow int
		destinationCol int
		tileMap        tileMap
		expectedPath   [][]int
	}

	tests := []test{
		{0, 1, 4, 3, tileMap{5, 5, []int{
			0, 1, 0, 0, 1,
			0, 2, 2, 3, 0,
			0, 1, 0, 1, 0,
			2, 0, 3, 4, 5,
			0, 0, 0, 1, 0,
		}}, [][]int{
			{0, 1},
			{0, 0},
			{1, 0},
			{2, 0},
			{2, 1},
			{3, 1},
			{4, 1},
			{4, 2},
			{4, 3},
		}},
		{0, 1, 0, 1, tileMap{5, 5, []int{
			0, 1, 0, 0, 1,
			0, 2, 2, 3, 0,
			0, 1, 0, 1, 0,
			2, 0, 3, 4, 5,
			0, 0, 0, 1, 0,
		}}, [][]int{}},
		{0, 1, 0, 2, tileMap{5, 5, []int{
			0, 1, 0, 0, 1,
			0, 2, 2, 3, 0,
			0, 1, 0, 1, 0,
			2, 0, 3, 4, 5,
			0, 0, 0, 1, 0,
		}}, [][]int{
			{0, 1},
			{0, 2},
		}},
		{1, 1, 7, 4, tileMap{10, 10, []int{
			2, 0, 0, 2, 1, 1, 1, 1, 1, 1,
			2, 0, 0, 2, 0, 0, 0, 1, 0, 0,
			2, 2, 0, 2, 1, 0, 0, 0, 0, 0,
			0, 1, 0, 0, 0, 0, 0, 1, 0, 0,
			1, 0, 0, 2, 1, 2, 2, 0, 0, 0,
			0, 1, 0, 2, 0, 0, 2, 0, 0, 0,
			0, 0, 0, 2, 1, 0, 2, 3, 3, 0,
			1, 0, 0, 3, 1, 0, 0, 0, 0, 0,
			0, 0, 0, 3, 3, 8, 0, 8, 8, 8,
			1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		}}, [][]int{
			{1, 1},
			{1, 2},
			{2, 2},
			{3, 2},
			{3, 3},
			{3, 4},
			{4, 4},
			{5, 4},
			{6, 4},
			{7, 4},
		}},
		{17, 16, 7, 0, tileMap{22, 20, []int{
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0,
			0, 2, 2, 0, 2, 2, 0, 2, 2, 2, 2, 2, 0, 2, 2, 2, 2, 2, 2, 0,
			0, 2, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0,
			0, 2, 0, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 2, 2, 2, 2, 0,
			0, 2, 0, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 0, 0, 0, 2, 2, 0,
			0, 2, 0, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 2, 2, 2, 0, 2, 2, 0,
			0, 2, 0, 0, 2, 0, 2, 0, 2, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 2, 0, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 2, 2, 2, 0, 0,
			0, 2, 0, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 0, 0, 0, 2, 0, 0,
			0, 2, 0, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 2, 2, 2, 0, 2, 0, 0,
			0, 2, 0, 0, 2, 0, 2, 0, 2, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 2, 0, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 2, 2, 2, 0, 0,
			0, 2, 0, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 0, 0, 0, 2, 0, 0,
			0, 2, 0, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 0, 0, 0, 2, 0, 0,
			0, 2, 0, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 0, 0, 0, 2, 0, 0,
			0, 2, 0, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 0, 0, 0, 2, 0, 0,
			0, 2, 0, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 0, 0, 0, 2, 0, 0,
			0, 2, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 2, 0, 0, 0, 0, 2, 0, 0,
			0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
			2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
		}}, [][]int{
			{17, 16},
			{16, 16},
			{16, 15},
			{16, 14},
			{16, 13},
			{15, 13},
			{14, 13},
			{13, 13},
			{12, 13},
			{11, 13},
			{10, 13},
			{10, 12},
			{10, 11},
			{9, 11},
			{8, 11},
			{7, 11},
			{6, 11},
			{5, 11},
			{4, 11},
			{3, 11},
			{2, 11},
			{2, 12},
			{1, 12},
			{0, 12},
			{0, 11},
			{0, 10},
			{0, 9},
			{0, 8},
			{0, 7},
			{0, 6},
			{0, 5},
			{0, 4},
			{0, 3},
			{0, 2},
			{0, 1},
			{0, 0},
			{1, 0},
			{2, 0},
			{3, 0},
			{4, 0},
			{5, 0},
			{6, 0},
			{7, 0},
		}},
	}

	for _, test := range tests {
		path, err := getShortedPathFromStartToDestination(test.startRow, test.startCol, test.destinationRow, test.destinationCol, test.tileMap)
		if err != nil {
			t.Errorf("getShortedPathFromStartToDestination returned error: %w", err)
		}
		if !reflect.DeepEqual(path, test.expectedPath) {
			t.Errorf("Path not equal to expected path.\nOutput path: %v\nExpected path: %v", path, test.expectedPath)
		}
	}
}