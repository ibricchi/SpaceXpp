package server

import (
	"fmt"
)

var tileWidth = 1

// Initilising starting map: unknown (1) with boarders (3)

var Map = tileMap{
	Rows: 12,
	Cols: 12,
	Tiles: []int{
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
		3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3,
		3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3,
		3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3,
		3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3,
		3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3,
		3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3,
		3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3,
		3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3,
		3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3,
		3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3,
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
}

//Initilising rover starting location: center of map facing to the right

var Rover = rover{
	X:        5,
	Y:        5,
	Rotation: 0,
}

func (h *HttpServer) mapAndDrive(destinationCol int, destinationRow int, mode int) error {

	// Getting optimum path
	path, err := getShortedPathFromStartToDestination(Rover.Y, Rover.Y, destinationRow, destinationCol, Map)

	if err != nil {
		return fmt.Errorf("Error: Failed to create path from start to destination  %w", err)
	}

	var direction direction = angle2Direction(Rover.Rotation)
	var traverseMode traverseMode = value2Mode(mode)

	driveInstruction, err := pathToDriveInstructions(path, tileWidth, direction, traverseMode)

	if err != nil {
		return fmt.Errorf("Error: Failed to create drive instructions  %w", err)
	}

	h.mqtt.publishDriveInstructionSequence(driveInstruction)

	return nil
}

func angle2Direction(angle int) direction {
	if angle == 0 {
		return east
	} else if angle == 90 {
		return south
	} else if angle == 180 {
		return west
	} else if angle == 270 {
		return north
	}
	return east
}

func value2Mode(mode int) traverseMode {
	if mode == 0 {
		return simple
	} else if mode == 1 {
		return fullDiscovery
	} else if mode == 2 {
		return destinationDiscovery
	}
	return simple
}

// Stashes latest instruction recived from rover and updates webpage with previous instruction

var stashedDriveInstruction driveInstruction

func updateMap(driveInstruction driveInstruction) {
	driveTocoords(stashedDriveInstruction, tileWidth)

	stashedDriveInstruction = driveInstruction

}

// To be implimented "stop"
/*
* Called when rover identifies obstruction
* Arguments:
*		- distance / location where obstruction is seen (to be confirmed)
*		- type of obstruction (to be confirmed)
* It will:
*		- update map with stashed instruction
*		- move distance moved before rover halted
*		- store nil instruction in stash
* 		- update map with location of obstruction & type of instruction
 */

func stop(distance int, obstructionType int) {

	driveTocoords(stashedDriveInstruction, tileWidth)

	stashedDriveInstruction.instruction = "forward"
	stashedDriveInstruction.value = distance
	driveTocoords(stashedDriveInstruction, tileWidth)

	stashedDriveInstruction.instruction = "forward"
	stashedDriveInstruction.value = 0

	// Assuming obstruction will only ever be in box in front
	// And for now only one type of instruction

	indx := getOneInFront()

	Map.Tiles[indx] = 3

}

func getOneInFront() int {
	if Rover.Rotation == 0 {
		return (Rover.X + 1) + (Rover.Y * Map.Cols)
	} else if Rover.Rotation == 180 {
		return (Rover.X - 1) + (Rover.Y * Map.Cols)
	} else if Rover.Rotation == 90 {
		return Rover.X + ((Rover.Y + 1) * Map.Cols)
	} else if Rover.Rotation == 270 {
		return Rover.X + ((Rover.Y - 1) * Map.Cols)
	}
	return 0
}
