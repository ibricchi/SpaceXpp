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
func pathToDriveInstructions(path [][]int, tileWidth int, initialDirection direction) ([]driveInstruction, error) {
	if len(path) < 2 {
		return nil, errors.New("server: drive: path must have at least two elements")
	}

	instructions := []driveInstruction{}

	// Distance moved forward since last turn
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

		if currentDirection == north && currentCol == previousCol { // Keep moving in same direction
			currentDistance += tileWidth
		} else if currentDirection == south && currentCol == previousCol { // Keep moving in same direction
			currentDistance += tileWidth
		} else if currentDirection == east && currentRow == previousRow { // Keep moving in same direction
			currentDistance += tileWidth
		} else if currentDirection == west && currentRow == previousRow { // Keep moving in same direction
			currentDistance += tileWidth
		} else { // Turn
			// Save forward instruction
			instructions = append(instructions, driveInstruction{
				instruction: "forward",
				value:       currentDistance,
			})

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
	}

	return instructions, nil
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
