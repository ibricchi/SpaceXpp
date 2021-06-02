package server

import (
	"errors"
	"fmt"

	"go.uber.org/zap"
)

var tileWidth = 10

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
	Rotation: 0, // angle (x-axis = 0°)
}

func (h *HttpServer) mapAndDrive(destinationCol int, destinationRow int, mode int) error {
	h.logger.Info("starting map and drive", zap.Int("startRow", Rover.Y), zap.Int("startCol", Rover.X), zap.Int("destinationRow", destinationRow), zap.Int("destinationCol", destinationCol))

	// Getting optimum path
	path, err := getShortedPathFromStartToDestination(Rover.X, Rover.Y, destinationRow, destinationCol, Map)
	if err != nil {
		return fmt.Errorf("server: map_general: mapAndDrive: failed to create path from start to destination: %w", err)
	}

	direction, err := angle2Direction(Rover.Rotation)
	if err != nil {
		return fmt.Errorf("server: map_general: mapAndDrive: failed to convert angle into direction: %w", err)
	}

	traverseMode, err := value2Mode(mode)
	if err != nil {
		return fmt.Errorf("server: map_general: mapAndDrive: failed to convert value into traversal mode: %w", err)
	}

	driveInstructions, err := pathToDriveInstructions(path, tileWidth, direction, traverseMode)
	if err != nil {
		return fmt.Errorf("server: map_general: mapAndDrive: failed to create drive instructions: %w", err)
	}

	h.mqtt.publishDriveInstructionSequence(driveInstructions)

	return nil
}

func angle2Direction(angle int) (direction, error) {
	if angle == 0 || angle == 360 {
		return east, nil
	} else if angle == 90 {
		return south, nil
	} else if angle == 180 {
		return west, nil
	} else if angle == 270 {
		return north, nil
	}
	return 0, errors.New("server: map_general: angle2Direction: angle does not match any direction")
}

func value2Mode(mode int) (traverseMode, error) {
	if mode == 0 {
		return simple, nil
	} else if mode == 1 {
		return fullDiscovery, nil
	} else if mode == 2 {
		return destinationDiscovery, nil
	}
	return 0, errors.New("server: map_general: value2Mode: mode is not a valid traverseMode")
}

// Stashes latest instruction recived from rover and updates webpage with previous instruction

var stashedDriveInstruction driveInstruction

func updateMap(driveInstruction driveInstruction) {
	driveTocoords(stashedDriveInstruction, tileWidth)

	stashedDriveInstruction = driveInstruction

}

// "stop"
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

func stop(distance int, obstructionType string) {

	driveTocoords(stashedDriveInstruction, tileWidth)

	stashedDriveInstruction.instruction = "forward"
	stashedDriveInstruction.value = distance
	driveTocoords(stashedDriveInstruction, tileWidth)

	stashedDriveInstruction.instruction = "forward"
	stashedDriveInstruction.value = 0

	// Assuming obstruction will only ever be in box in front
	// And for now only one type of instruction

	indx := getOneInFront()

	Map.Tiles[indx] = obstacleToValue(obstructionType)

}

func obstacleToValue(obstacle string) int {

	if obstacle == "B" {
		return 6
	} else if obstacle == "R" {
		return 7
	} else if obstacle == "Y" {
		return 8
	} else if obstacle == "T" {
		return 9
	} else if obstacle == "V" {
		return 10
	}

	return 5
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
