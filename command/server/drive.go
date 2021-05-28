package server

import (
	"errors"
	"fmt"
)

/*
	Types of instructions:
	1.) forward: distance (cm)
	2.) turnRight: angle (degrees)
	3.) turnLeft: angle (degrees)
*/
type driveInstruction struct {
	instruction string
	value       int
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

		turnInstruction, turnAngle, err := getTurnInstructionFromAngle(angle)
		if err != nil {
			return nil, fmt.Errorf("server: drive: failed to get turn instruction from angle: %w", err)
		}
		instructions = append(instructions, driveInstruction{
			instruction: turnInstruction,
			value:       turnAngle,
		})
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
					instruction: "forward",
					value:       currentDistance,
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
				instruction: "forward",
				value:       currentDistance,
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

			turnInstruction, turnAngle, err := getTurnInstructionFromAngle(angle)
			if err != nil {
				return nil, fmt.Errorf("server: drive: failed to get turn instruction from angle: %w", err)
			}
			instructions = append(instructions, driveInstruction{
				instruction: turnInstruction,
				value:       turnAngle,
			})

			currentDirection = newDirection
		}

		// Last forward instruction (not terminated by a turn)
		if i == len(path)-1 {
			instructions = append(instructions, driveInstruction{
				instruction: "forward",
				value:       currentDistance,
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
	turnRight instruction is used to handle turns of 180°.
*/
func getTurnInstructionFromAngle(angle int) (string, int, error) {
	switch angle {
	case 90:
		return "turnRight", 90, nil
	case 180:
		return "turnRight", 180, nil
	case 270:
		return "turnLeft", 90, nil
	default:
		return "", 0, fmt.Errorf("server: drive: invalid turn angle of %v degrees", angle)
	}
}

// Converting drive instruction into the coordinates that the rover will end up in

func driveTocoords(driveInstruction driveInstruction, tileWidth int) {

	if driveInstruction.instruction == "forward" {
		if Rover.Rotation == 0 {
			Rover.X = Rover.X + (driveInstruction.value / tileWidth)
		} else if Rover.Rotation == 180 {
			Rover.X = Rover.X - (driveInstruction.value / tileWidth)
		} else if Rover.Rotation == 90 {
			Rover.Y = Rover.Y + (driveInstruction.value / tileWidth)
		} else if Rover.Rotation == 270 {
			Rover.Y = Rover.Y - (driveInstruction.value / tileWidth)
		}
	} else if driveInstruction.instruction == "turnRight" {
		Rover.Rotation = (Rover.Rotation + driveInstruction.value) % 360
	} else if driveInstruction.instruction == "turnLeft" {
		Rover.Rotation = (360 + ((Rover.Rotation - driveInstruction.value) % 360)) % 360
	}
}
