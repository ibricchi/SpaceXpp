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
					Instruction: "forward",
					Value:       10,
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
					Instruction: "turnRight",
					Value:       90,
				},
				{
					Instruction: "forward",
					Value:       10,
				},
			},
		},
		{
			path: [][]int{
				{0, 1},
				{0, 2},
			},
			tileWidth:        10,
			initialDirection: east,
			traverseMode:     destinationDiscovery,
			expectedInstructions: []driveInstruction{
				{
					Instruction: "forward",
					Value:       10,
				},
				{
					Instruction: "turnRight",
					Value:       90,
				},
				{
					Instruction: "turnLeft",
					Value:       90,
				},
				{
					Instruction: "turnLeft",
					Value:       90,
				},
				{
					Instruction: "turnRight",
					Value:       90,
				},
			},
		},
		{
			path: [][]int{
				{0, 1},
				{0, 2},
			},
			tileWidth:        10,
			initialDirection: east,
			traverseMode:     fullDiscovery,
			expectedInstructions: []driveInstruction{
				{
					Instruction: "forward",
					Value:       10,
				},
				{
					Instruction: "turnRight",
					Value:       90,
				},
				{
					Instruction: "turnLeft",
					Value:       90,
				},
				{
					Instruction: "turnLeft",
					Value:       90,
				},
				{
					Instruction: "turnRight",
					Value:       90,
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
					Instruction: "turnRight",
					Value:       90,
				},
				{
					Instruction: "turnRight",
					Value:       90,
				},
				{
					Instruction: "forward",
					Value:       10,
				},
				{
					Instruction: "turnLeft",
					Value:       90,
				},
				{
					Instruction: "forward",
					Value:       20,
				},
				{
					Instruction: "turnLeft",
					Value:       90,
				},
				{
					Instruction: "forward",
					Value:       10,
				},
				{
					Instruction: "turnRight",
					Value:       90,
				},
				{
					Instruction: "forward",
					Value:       20,
				},
				{
					Instruction: "turnLeft",
					Value:       90,
				},
				{
					Instruction: "forward",
					Value:       30,
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
					Instruction: "forward",
					Value:       10,
				},
				{
					Instruction: "turnLeft",
					Value:       90,
				},
				{
					Instruction: "forward",
					Value:       20,
				},
				{
					Instruction: "turnLeft",
					Value:       90,
				},
				{
					Instruction: "forward",
					Value:       10,
				},
				{
					Instruction: "turnRight",
					Value:       90,
				},
				{
					Instruction: "forward",
					Value:       20,
				},
				{
					Instruction: "turnLeft",
					Value:       90,
				},
				{
					Instruction: "forward",
					Value:       60,
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
					Instruction: "turnRight",
					Value:       90,
				},
				{
					Instruction: "turnRight",
					Value:       90,
				},
				{
					Instruction: "forward",
					Value:       10,
				},
				{
					Instruction: "turnRight",
					Value:       90,
				},
				{
					Instruction: "turnLeft",
					Value:       90,
				},
				{
					Instruction: "turnLeft",
					Value:       90,
				},
				{
					Instruction: "turnRight",
					Value:       90,
				},
				{
					Instruction: "turnLeft",
					Value:       90,
				},
				{
					Instruction: "forward",
					Value:       10,
				},
				{
					Instruction: "turnRight",
					Value:       90,
				},
				{
					Instruction: "turnLeft",
					Value:       90,
				},
				{
					Instruction: "turnLeft",
					Value:       90,
				},
				{
					Instruction: "turnRight",
					Value:       90,
				},
				{
					Instruction: "forward",
					Value:       10,
				},
				{
					Instruction: "turnRight",
					Value:       90,
				},
				{
					Instruction: "turnLeft",
					Value:       90,
				},
				{
					Instruction: "turnLeft",
					Value:       90,
				},
				{
					Instruction: "turnRight",
					Value:       90,
				},
				{
					Instruction: "forward",
					Value:       10,
				},
				{
					Instruction: "turnRight",
					Value:       90,
				},
				{
					Instruction: "turnLeft",
					Value:       90,
				},
				{
					Instruction: "turnLeft",
					Value:       90,
				},
				{
					Instruction: "turnRight",
					Value:       90,
				},
				{
					Instruction: "turnLeft",
					Value:       90,
				},
				{
					Instruction: "forward",
					Value:       10,
				},
				{
					Instruction: "turnRight",
					Value:       90,
				},
				{
					Instruction: "turnLeft",
					Value:       90,
				},
				{
					Instruction: "turnLeft",
					Value:       90,
				},
				{
					Instruction: "turnRight",
					Value:       90,
				},
				{
					Instruction: "turnRight",
					Value:       90,
				},
				{
					Instruction: "forward",
					Value:       10,
				},
				{
					Instruction: "turnRight",
					Value:       90,
				},
				{
					Instruction: "turnLeft",
					Value:       90,
				},
				{
					Instruction: "turnLeft",
					Value:       90,
				},
				{
					Instruction: "turnRight",
					Value:       90,
				},
				{
					Instruction: "forward",
					Value:       10,
				},
				{
					Instruction: "turnRight",
					Value:       90,
				},
				{
					Instruction: "turnLeft",
					Value:       90,
				},
				{
					Instruction: "turnLeft",
					Value:       90,
				},
				{
					Instruction: "turnRight",
					Value:       90,
				},
				{
					Instruction: "turnLeft",
					Value:       90,
				},
				{
					Instruction: "forward",
					Value:       10,
				},
				{
					Instruction: "turnRight",
					Value:       90,
				},
				{
					Instruction: "turnLeft",
					Value:       90,
				},
				{
					Instruction: "turnLeft",
					Value:       90,
				},
				{
					Instruction: "turnRight",
					Value:       90,
				},
				{
					Instruction: "forward",
					Value:       10,
				},
				{
					Instruction: "turnRight",
					Value:       90,
				},
				{
					Instruction: "turnLeft",
					Value:       90,
				},
				{
					Instruction: "turnLeft",
					Value:       90,
				},
				{
					Instruction: "turnRight",
					Value:       90,
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
					Instruction: "turnRight",
					Value:       90,
				},
				{
					Instruction: "turnRight",
					Value:       90,
				},
				{
					Instruction: "forward",
					Value:       10,
				},
				{
					Instruction: "turnLeft",
					Value:       90,
				},
				{
					Instruction: "forward",
					Value:       20,
				},
				{
					Instruction: "turnLeft",
					Value:       90,
				},
				{
					Instruction: "forward",
					Value:       10,
				},
				{
					Instruction: "turnRight",
					Value:       90,
				},
				{
					Instruction: "forward",
					Value:       20,
				},
				{
					Instruction: "turnLeft",
					Value:       90,
				},
				{
					Instruction: "forward",
					Value:       30,
				},
				{
					Instruction: "turnRight",
					Value:       90,
				},
				{
					Instruction: "turnLeft",
					Value:       90,
				},
				{
					Instruction: "turnLeft",
					Value:       90,
				},
				{
					Instruction: "turnRight",
					Value:       90,
				},
			},
		},
	}

	for _, test := range tests {
		Instructions, err := pathToDriveInstructions(test.path, test.tileWidth, test.initialDirection, test.traverseMode)
		if err != nil {
			t.Errorf("pathToDriveInstructions returned error: %w", err)
		}
		if !reflect.DeepEqual(Instructions, test.expectedInstructions) {
			t.Errorf("Instructions not equal to expected Instructions.\nOutput Instructions: %v\nExpected Instructions: %v", Instructions, test.expectedInstructions)
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
