package server

import "fmt"

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
		3, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 3,
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

	fmt.Println("mode:", mode)

	// Getting optimum path
	path, err := getShortedPathFromStartToDestination(Rover.Y, Rover.Y, destinationRow, destinationCol, Map)

	if err != nil {
		return fmt.Errorf("Error: Failed to create path from start to destination  %w", err)
	}

	fmt.Println("optimum path found")

	var direction direction = angle2Direction(Rover.Rotation)
	var traverseMode traverseMode = value2Mode(mode)

	driveInstruction, err := pathToDriveInstructions(path, 64, direction, traverseMode)

	if err != nil {
		return fmt.Errorf("Error: Failed to create drive instructions  %w", err)
	}

	fmt.Println("drive instructions created")

	h.mqtt.publishDriveInstructionSequence(driveInstruction)

	fmt.Println("drive instructions printed")

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
