package server

import (
	"reflect"
	"testing"
)

func TestPathToDriveInstructions(t *testing.T) {
	type test struct {
		path                 [][]int
		tileWidth            int
		initialDirection     direction
		traverseMode         traverseMode
		expectedInstructions []driveInstruction
	}

	tests := []test{
		{
			path: [][]int{
				{0, 1},
				{0, 2},
			},
			tileWidth:        10,
			initialDirection: east,
			traverseMode:     simple,
			expectedInstructions: []driveInstruction{
				{
					instruction: "forward",
					value:       10,
				},
			},
		},
		{
			path: [][]int{
				{0, 1},
				{0, 2},
			},
			tileWidth:        10,
			initialDirection: north,
			traverseMode:     simple,
			expectedInstructions: []driveInstruction{
				{
					instruction: "turnRight",
					value:       90,
				},
				{
					instruction: "forward",
					value:       10,
				},
			},
		},
		{
			path: [][]int{
				{0, 1},
				{0, 0},
				{1, 0},
				{2, 0},
				{2, 1},
				{3, 1},
				{4, 1},
				{4, 2},
				{4, 3},
				{4, 4},
			},
			tileWidth:        10,
			initialDirection: east,
			traverseMode:     simple,
			expectedInstructions: []driveInstruction{
				{
					instruction: "turnRight",
					value:       180,
				},
				{
					instruction: "forward",
					value:       10,
				},
				{
					instruction: "turnLeft",
					value:       90,
				},
				{
					instruction: "forward",
					value:       20,
				},
				{
					instruction: "turnLeft",
					value:       90,
				},
				{
					instruction: "forward",
					value:       10,
				},
				{
					instruction: "turnRight",
					value:       90,
				},
				{
					instruction: "forward",
					value:       20,
				},
				{
					instruction: "turnLeft",
					value:       90,
				},
				{
					instruction: "forward",
					value:       30,
				},
			},
		},
		{
			path: [][]int{
				{0, 1},
				{0, 0},
				{1, 0},
				{2, 0},
				{2, 1},
				{3, 1},
				{4, 1},
				{4, 2},
				{4, 3},
				{4, 4},
				{4, 5},
				{4, 6},
				{4, 7},
			},
			tileWidth:        10,
			initialDirection: west,
			traverseMode:     simple,
			expectedInstructions: []driveInstruction{
				{
					instruction: "forward",
					value:       10,
				},
				{
					instruction: "turnLeft",
					value:       90,
				},
				{
					instruction: "forward",
					value:       20,
				},
				{
					instruction: "turnLeft",
					value:       90,
				},
				{
					instruction: "forward",
					value:       10,
				},
				{
					instruction: "turnRight",
					value:       90,
				},
				{
					instruction: "forward",
					value:       20,
				},
				{
					instruction: "turnLeft",
					value:       90,
				},
				{
					instruction: "forward",
					value:       60,
				},
			},
		},
		{
			path: [][]int{
				{0, 1},
				{0, 0},
				{1, 0},
				{2, 0},
				{3, 0},
				{3, 1},
				{4, 1},
				{5, 1},
				{5, 2},
				{5, 3},
			},
			tileWidth:        10,
			initialDirection: east,
			traverseMode:     fullDiscovery,
			expectedInstructions: []driveInstruction{
				{
					instruction: "turnRight",
					value:       180,
				},
				{
					instruction: "forward",
					value:       10,
				},
				{
					instruction: "turnRight",
					value:       90,
				},
				{
					instruction: "turnRight",
					value:       90,
				},
				{
					instruction: "turnRight",
					value:       90,
				},
				{
					instruction: "turnRight",
					value:       90,
				},
				{
					instruction: "turnLeft",
					value:       90,
				},
				{
					instruction: "forward",
					value:       10,
				},
				{
					instruction: "turnRight",
					value:       90,
				},
				{
					instruction: "turnRight",
					value:       90,
				},
				{
					instruction: "turnRight",
					value:       90,
				},
				{
					instruction: "turnRight",
					value:       90,
				},
				{
					instruction: "forward",
					value:       10,
				},
				{
					instruction: "turnRight",
					value:       90,
				},
				{
					instruction: "turnRight",
					value:       90,
				},
				{
					instruction: "turnRight",
					value:       90,
				},
				{
					instruction: "turnRight",
					value:       90,
				},
				{
					instruction: "forward",
					value:       10,
				},
				{
					instruction: "turnRight",
					value:       90,
				},
				{
					instruction: "turnRight",
					value:       90,
				},
				{
					instruction: "turnRight",
					value:       90,
				},
				{
					instruction: "turnRight",
					value:       90,
				},
				{
					instruction: "turnLeft",
					value:       90,
				},
				{
					instruction: "forward",
					value:       10,
				},
				{
					instruction: "turnRight",
					value:       90,
				},
				{
					instruction: "turnRight",
					value:       90,
				},
				{
					instruction: "turnRight",
					value:       90,
				},
				{
					instruction: "turnRight",
					value:       90,
				},
				{
					instruction: "turnRight",
					value:       90,
				},
				{
					instruction: "forward",
					value:       10,
				},
				{
					instruction: "turnRight",
					value:       90,
				},
				{
					instruction: "turnRight",
					value:       90,
				},
				{
					instruction: "turnRight",
					value:       90,
				},
				{
					instruction: "turnRight",
					value:       90,
				},
				{
					instruction: "forward",
					value:       10,
				},
				{
					instruction: "turnRight",
					value:       90,
				},
				{
					instruction: "turnRight",
					value:       90,
				},
				{
					instruction: "turnRight",
					value:       90,
				},
				{
					instruction: "turnRight",
					value:       90,
				},
				{
					instruction: "turnLeft",
					value:       90,
				},
				{
					instruction: "forward",
					value:       10,
				},
				{
					instruction: "turnRight",
					value:       90,
				},
				{
					instruction: "turnRight",
					value:       90,
				},
				{
					instruction: "turnRight",
					value:       90,
				},
				{
					instruction: "turnRight",
					value:       90,
				},
				{
					instruction: "forward",
					value:       10,
				},
				{
					instruction: "turnRight",
					value:       90,
				},
				{
					instruction: "turnRight",
					value:       90,
				},
				{
					instruction: "turnRight",
					value:       90,
				},
				{
					instruction: "turnRight",
					value:       90,
				},
			},
		},
		{
			path: [][]int{
				{0, 1},
				{0, 0},
				{1, 0},
				{2, 0},
				{2, 1},
				{3, 1},
				{4, 1},
				{4, 2},
				{4, 3},
				{4, 4},
			},
			tileWidth:        10,
			initialDirection: east,
			traverseMode:     destinationDiscovery,
			expectedInstructions: []driveInstruction{
				{
					instruction: "turnRight",
					value:       180,
				},
				{
					instruction: "forward",
					value:       10,
				},
				{
					instruction: "turnLeft",
					value:       90,
				},
				{
					instruction: "forward",
					value:       20,
				},
				{
					instruction: "turnLeft",
					value:       90,
				},
				{
					instruction: "forward",
					value:       10,
				},
				{
					instruction: "turnRight",
					value:       90,
				},
				{
					instruction: "forward",
					value:       20,
				},
				{
					instruction: "turnLeft",
					value:       90,
				},
				{
					instruction: "forward",
					value:       30,
				},
				{
					instruction: "turnRight",
					value:       90,
				},
				{
					instruction: "turnRight",
					value:       90,
				},
				{
					instruction: "turnRight",
					value:       90,
				},
				{
					instruction: "turnRight",
					value:       90,
				},
			},
		},
	}

	for _, test := range tests {
		instructions, err := pathToDriveInstructions(test.path, test.tileWidth, test.initialDirection, test.traverseMode)
		if err != nil {
			t.Errorf("pathToDriveInstructions returned error: %w", err)
		}
		if !reflect.DeepEqual(instructions, test.expectedInstructions) {
			t.Errorf("Instructions not equal to expected instructions.\nOutput instructions: %v\nExpected instructions: %v", instructions, test.expectedInstructions)
		}
	}
}

func TestGetNewDirection(t *testing.T) {
	type test struct {
		currentNode       []int
		nextNode          []int
		expectedDirection direction
	}

	tests := []test{
		{
			currentNode:       []int{0, 1},
			nextNode:          []int{0, 0},
			expectedDirection: west,
		},
		{
			currentNode:       []int{0, 0},
			nextNode:          []int{1, 0},
			expectedDirection: south,
		},
		{
			currentNode:       []int{2, 0},
			nextNode:          []int{2, 1},
			expectedDirection: east,
		},
		{
			currentNode:       []int{4, 1},
			nextNode:          []int{3, 1},
			expectedDirection: north,
		},
	}

	for _, test := range tests {
		if direction := getNewDirection(test.currentNode, test.nextNode); direction != test.expectedDirection {
			t.Errorf("Direction not equal to expected direction.\nOutput direction: %v\nExpected direction: %v", direction, test.expectedDirection)
		}
	}
}

func TestGetTurnAngleClockwise(t *testing.T) {
	type test struct {
		currentDirection direction
		newDirection     direction
		expectedAngle    int
	}

	tests := []test{
		{
			currentDirection: north,
			newDirection:     north,
			expectedAngle:    0,
		},
		{
			currentDirection: north,
			newDirection:     east,
			expectedAngle:    90,
		},
		{
			currentDirection: north,
			newDirection:     west,
			expectedAngle:    270,
		},
		{
			currentDirection: south,
			newDirection:     north,
			expectedAngle:    180,
		},
		{
			currentDirection: east,
			newDirection:     north,
			expectedAngle:    270,
		},
		{
			currentDirection: west,
			newDirection:     east,
			expectedAngle:    180,
		},
		{
			currentDirection: west,
			newDirection:     north,
			expectedAngle:    90,
		},
	}

	for _, test := range tests {
		if angle := getTurnAngleClockwise(test.currentDirection, test.newDirection); angle != test.expectedAngle {
			t.Errorf("Angle not equal to expected angle.\nOutput angle: %v\nExpected angle: %v", angle, test.expectedAngle)
		}
	}
}
