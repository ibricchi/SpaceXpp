package server

import (
	"errors"
	"fmt"

	"go.uber.org/zap/zapcore"
)

/*
	Types of instructions:
	1.) forward: distance (cm)
	2.) turnRight: angle (degrees)
	3.) turnLeft: angle (degrees)
*/
type driveInstruction struct {
	Instruction string `json:"instruction"`
	Value       int    `json:"value"`
}

// For greater control on map building using on-board camera
type traverseMode int

const (
	simple               traverseMode = iota // Traverse the path without extra rotations
	fullDiscovery                            // Traverse the path and complete a full rotation on each tile
	destinationDiscovery                     // Traverse the path and complete a full roation on the destination tile
)

// Direction on tile map that rover is facing
type direction int

const (
	north direction = iota
	south
	east
	west
)

func (i *driveInstruction) MarshalLogObject(enc zapcore.ObjectEncoder) error {
	enc.AddString("instruction", i.Instruction)
	enc.AddInt("value", i.Value)
	return nil
}

type driveInstructions []driveInstruction

func (is *driveInstructions) MarshalLogArray(enc zapcore.ArrayEncoder) error {
	for _, instruction := range *is {
		if err := enc.AppendObject(&instruction); err != nil {
			return fmt.Errorf("server: drive: failed to encode driveInstructions: %w", err)
		}
	}
	return nil
}

/*
	Takes a path represented as a list of [row, col] pairs and returns a sequence of drive instructions.
	The path contains both the start tile (node) and the destination tile. No drive instructions are needed to get to the start node.

	tileWidth is the height/width of a tile in cm. Every element in the path increases the total distance by tileWidth (apart from the first one).
	initialDirection is the direction on the tile map that the rover is facing initially.

	turnRight instruction is used to handle turns of 180°.

	Time complexity: O(n) where n = number of nodes in path
*/
func pathToDriveInstructions(path [][]int, tileWidth int, initialDirection direction, traverseMode traverseMode) ([]driveInstruction, error) {
	if len(path) < 2 {
		return nil, errors.New("server: drive: path must have at least two elements")
	}

	instructions := []driveInstruction{}

	// Distance moved forward since last turn (always equal to tileWidth for fullDiscovery traversal mode)
	currentDistance := tileWidth
	// Direction on tile map that rover is currently facing (use start tile to determine initial direction)
	currentDirection := getNewDirection(path[0], path[1])

	// Start by turning
	if currentDirection != initialDirection {
		angle := getTurnAngleClockwise(initialDirection, currentDirection)

		turnInstructions, err := getTurnInstructionsFromAngle(angle)
		if err != nil {
			return nil, fmt.Errorf("server: drive: failed to get turn instruction from angle: %w", err)
		}
		instructions = append(instructions, turnInstructions...)
	}

	// Check for special case of only one forward instruction
	if len(path) == 2 {
		instructions = append(instructions, driveInstruction{
			Instruction: "forward",
			Value:       currentDistance,
		})

		if traverseMode == fullDiscovery || traverseMode == destinationDiscovery {
			// Add instructions for full roation
			instructions = append(instructions, getInstructionForFullRotation(currentDirection)...)
		}
	}

	// Skip start tile (already there)
	for i := 2; i < len(path); i++ {
		currentRow := path[i][0]
		currentCol := path[i][1]
		previousRow := path[i-1][0]
		previousCol := path[i-1][1]

		if (currentDirection == north && currentCol == previousCol) ||
			(currentDirection == south && currentCol == previousCol) ||
			(currentDirection == east && currentRow == previousRow) ||
			(currentDirection == west && currentRow == previousRow) { // Keep moving in same direction
			if traverseMode == fullDiscovery {
				// Add forward instruction
				instructions = append(instructions, driveInstruction{
					Instruction: "forward",
					Value:       currentDistance,
				})

				// Add instructions for full roation
				instructions = append(instructions, getInstructionForFullRotation(currentDirection)...)
			} else {
				// Group multiple forward instructions together
				currentDistance += tileWidth
			}
		} else { // Turn
			// Save grouped forward instruction
			instructions = append(instructions, driveInstruction{
				Instruction: "forward",
				Value:       currentDistance,
			})
			if traverseMode == fullDiscovery {
				// Add instructions for full roation as rotation leads to moving one forward
				instructions = append(instructions, getInstructionForFullRotation(currentDirection)...)
			}

			// Automatically move one forward after turn
			currentDistance = tileWidth

			// Compute turn instruction
			newDirection := getNewDirection(path[i-1], path[i])
			angle := getTurnAngleClockwise(currentDirection, newDirection)

			turnInstructions, err := getTurnInstructionsFromAngle(angle)
			if err != nil {
				return nil, fmt.Errorf("server: drive: failed to get turn instruction from angle: %w", err)
			}
			instructions = append(instructions, turnInstructions...)

			currentDirection = newDirection
		}

		// Last forward instruction (not terminated by a turn)
		if i == len(path)-1 {
			instructions = append(instructions, driveInstruction{
				Instruction: "forward",
				Value:       currentDistance,
			})
		}

		// Add full rotation once destination is reached
		if i == len(path)-1 && traverseMode != simple {
			instructions = append(instructions, getInstructionForFullRotation(currentDirection)...)
		}
	}

	return instructions, nil
}

// Instructions for full rotation in 90° steps to allow vision to obtain images of all surrounding tiles
func getInstructionForFullRotation(initialDirection direction) []driveInstruction {
	return []driveInstruction{
		{
			Instruction: "turnRight",
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
			Instruction: "turnRight",
			Value:       90,
		},
	}
}

func getNewDirection(currenNode []int, nextNode []int) direction {
	currentRow := currenNode[0]
	currentCol := currenNode[1]
	nextRow := nextNode[0]
	nextCol := nextNode[1]

	if currentCol == nextCol && currentRow-1 == nextRow {
		return north
	}
	if currentCol == nextCol && currentRow+1 == nextRow {
		return south
	}
	if currentRow == nextRow && currentCol+1 == nextCol {
		return east
	}
	return west
}

// Returns an angle in degrees between 0° and 360°
func getTurnAngleClockwise(currentDirection direction, newDirection direction) int {
	directionVals := map[direction]int{
		north: 0,
		east:  1,
		south: 2,
		west:  3,
	}

	valDifference := directionVals[newDirection] - directionVals[currentDirection]
	if valDifference < 0 {
		valDifference += 4
	}

	return 90 * valDifference
}

/*
	Turns must be a multiple of 90°.
	turnRight instruction is used to handle turns of 180° (split into two 90° instructions).
*/
func getTurnInstructionsFromAngle(angle int) ([]driveInstruction, error) {
	instructions := []driveInstruction{}
	switch angle {
	case 90:
		instructions = append(instructions, driveInstruction{Instruction: "turnRight", Value: 90})
	case 180:
		instructions = append(instructions, driveInstruction{Instruction: "turnRight", Value: 90}, driveInstruction{Instruction: "turnRight", Value: 90})
	case 270:
		instructions = append(instructions, driveInstruction{Instruction: "turnLeft", Value: 90})
	default:
		return []driveInstruction{}, fmt.Errorf("server: drive: invalid turn angle of %v degrees", angle)
	}

	return instructions, nil
}

// Converting drive instruction into the coordinates that the rover will end up in

func driveTocoords(driveInstruction driveInstruction, tileWidth int) {

	if driveInstruction.Instruction == "forward" {
		if Rover.Rotation == 0 {
			end := Rover.X + (driveInstruction.Value / tileWidth)
			changeTerrainX(Rover.X, Rover.Y, end)
			Rover.X = end

		} else if Rover.Rotation == 180 {
			end := Rover.X - (driveInstruction.Value / tileWidth)
			changeTerrainX(end, Rover.Y, Rover.X)
			Rover.X = end
		} else if Rover.Rotation == 90 {
			end := Rover.Y + (driveInstruction.Value / tileWidth)
			changeTerrainY(Rover.X, Rover.Y, end)
			Rover.Y = end
		} else if Rover.Rotation == 270 {
			end := Rover.Y - (driveInstruction.Value / tileWidth)
			changeTerrainY(Rover.X, end, Rover.Y)
			Rover.Y = end
		}
	} else if driveInstruction.Instruction == "turnRight" {
		Rover.Rotation = (Rover.Rotation + driveInstruction.Value) % 360
	} else if driveInstruction.Instruction == "turnLeft" {
		Rover.Rotation = (360 + ((Rover.Rotation - driveInstruction.Value) % 360)) % 360
	}
}

func changeTerrainX(startX int, y int, endX int) {

	s := startX + (y * Map.Cols)
	e := endX + (y * Map.Cols)

	for i := s; i <= e; i++ {
		Map.Tiles[i] = 2
	}
}

func changeTerrainY(x int, startY int, endY int) {

	s := x + (startY * Map.Cols)
	e := x + (endY * Map.Cols)

	for i := s; i <= e; i = i + 12 {
		Map.Tiles[i] = 2
	}
}
