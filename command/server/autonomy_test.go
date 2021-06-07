package server

import (
	"reflect"
	"sort"
	"testing"
)

func TestGetUndiscoveredRectangles(t *testing.T) {
	type test struct {
		tileMap            tileMap
		expectedRectangles []rectangle
	}

	tests := []test{
		{
			tileMap: tileMap{
				Rows: 17,
				Cols: 17,
				Tiles: []int{
					0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0,
					0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 0,
					0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0,
					0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 2,
					0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 2,
					0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0,
					0, 0, 2, 2, 2, 0, 1, 1, 0, 9, 0, 0, 1, 1, 1, 1, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0,
					0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0,
					0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0,
					0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0,
					0, 0, 0, 0, 0, 6, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0,
				},
			},
			expectedRectangles: []rectangle{
				{
					left:   10,
					right:  13,
					top:    1,
					bottom: 5,
				},
				{
					left:   15,
					right:  15,
					top:    1,
					bottom: 3,
				},
				{
					left:   1,
					right:  4,
					top:    3,
					bottom: 6,
				},
				{
					left:   6,
					right:  7,
					top:    3,
					bottom: 8,
				},
				{
					left:   12,
					right:  15,
					top:    7,
					bottom: 10,
				},
				{
					left:   3,
					right:  10,
					top:    10,
					bottom: 13,
				},
				{
					left:   13,
					right:  15,
					top:    13,
					bottom: 14,
				},
				{
					left:   1,
					right:  1,
					top:    14,
					bottom: 14,
				},
			},
		},
		// {
		// 	tileMap: tileMap{
		// 		Rows: 10,
		// 		Cols: 10,
		// 		Tiles: []int{ // Touching rectangles
		// 			5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
		// 			5, 1, 1, 0, 0, 0, 0, 1, 1, 5,
		// 			5, 1, 1, 1, 1, 0, 0, 8, 1, 5,
		// 			5, 1, 1, 1, 1, 0, 0, 0, 0, 5,
		// 			5, 1, 1, 1, 1, 0, 1, 1, 1, 5,
		// 			5, 0, 1, 1, 0, 0, 1, 1, 0, 5,
		// 			5, 0, 2, 1, 0, 1, 1, 1, 1, 5,
		// 			5, 1, 1, 3, 0, 0, 1, 1, 1, 5,
		// 			5, 1, 0, 0, 0, 0, 1, 1, 0, 5,
		// 			5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
		// 		},
		// 	},
		// 	expectedRectangles: []rectangle{
		// 	},
		// },
	}

	for _, test := range tests {
		rectangles := getUndiscoveredRectangles(test.tileMap)

		// Order does not matter
		sort.Slice(rectangles, func(i int, j int) bool {
			if rectangles[i].left == rectangles[j].left {
				return rectangles[i].top < rectangles[j].top
			}
			return rectangles[i].left < rectangles[j].left
		})
		sort.Slice(test.expectedRectangles, func(i int, j int) bool {
			if test.expectedRectangles[i].left == test.expectedRectangles[j].left {
				return test.expectedRectangles[i].top < test.expectedRectangles[j].top
			}
			return test.expectedRectangles[i].left < test.expectedRectangles[j].left
		})

		if !reflect.DeepEqual(rectangles, test.expectedRectangles) {
			t.Errorf("Rectangles not equal to expected rectangles.\nOutput rectangles: %v\nExpected rectangles: %v", rectangles, test.expectedRectangles)
		}
	}
}
