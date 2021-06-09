package server

import (
	"context"
	"errors"
	"fmt"
	"strconv"

	"go.uber.org/zap"
)

var tileWidth = 30

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

// Used for case when having to recompute path due to obstruction avoidance
var previousDestinationRow int
var previousDestinationCol int
var previousDestinationMode int

// Used to record feedback
var feed string

// Used to stop autonomous
var stopAutonomous bool = true

// Used to store current energy readings
var currentEnergy = energy{
	StateOfCharge: 0,
	StateOfHealth: 0,
	ErrorInCells:  0,
}

func mapAndDrive(mqtt MQTT, destinationCol int, destinationRow int, mode int) error {
	mqtt.getLogger().Info("starting map and drive", zap.Int("startRow", Rover.Y), zap.Int("startCol", Rover.X), zap.Int("destinationRow", destinationRow), zap.Int("destinationCol", destinationCol))

	// Getting optimum path
	path, err := getShortedPathFromStartToDestination(Rover.Y, Rover.X, destinationRow, destinationCol, Map)
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

	mqtt.publishDriveInstructionSequence(driveInstructions)

	feed = "<br> <br> Drive instructions sent to rover <br> <br> Optimum path converted to drive instructions <br> <br> Targets converted to optimum path <br> <br> Targets recived by server " + feed

	return nil
}

func autonomousDrive(mqtt MQTT) {
	available, x, y := getBestNextDestinationCoordinates(Map)
	if available == false || stopAutonomous == false {
		mapAndDrive(mqtt, x, y, 1)
	}

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

func updateMap(driveInstruction driveInstruction, ctx context.Context, db DB) {

	feed = " <br> <br> Instruction : " + driveInstruction.Instruction + ":" + strconv.Itoa(driveInstruction.Value) + " : Sucsessful" + feed

	fmt.Println("inserting instructions via update map")

	if err := db.storeInstruction(ctx, driveInstruction.Instruction, driveInstruction.Value); err != nil {
		db.getLogger().Error("server: map_general: updateMap: failed to store instruction", zap.Error(err))
	}

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
* 		- update map with location of obstruction & type of instruction (optionally based on updateMap argument)
 */

func stop(mqtt MQTT, ctx context.Context, db DB, distance int, obstructionType string, stopAfterTurn bool) {
	feed = "Obstruction identified"

	if stopAfterTurn {
		feed = "<br> <br> Instruction : " + stashedDriveInstruction.Instruction + ":" + strconv.Itoa(stashedDriveInstruction.Value) + " : Sucsessful <br> <br> Obstruction not on path, continuing to move <br> <br>" + feed

		// Complete turn
		driveTocoords(stashedDriveInstruction, tileWidth)
	} else {

		// Drive forward distance moved before stopping
		stashedDriveInstruction.Instruction = "forward"
		stashedDriveInstruction.Value = distance

		feed = "<br> <br> Adjusted instruction : " + stashedDriveInstruction.Instruction + ":" + strconv.Itoa(stashedDriveInstruction.Value) + " : Sucsessful <br> <br> Obstruction on path, adjusting instruction " + feed

		if err := db.storeInstruction(ctx, stashedDriveInstruction.Instruction, stashedDriveInstruction.Value); err != nil {
			mqtt.getLogger().Error("server: map_general: stop: failed to store instruction", zap.Error(err))
		}

		driveTocoords(stashedDriveInstruction, tileWidth)
	}

	// Store nil instruction in stash
	stashedDriveInstruction.Instruction = "forward"
	stashedDriveInstruction.Value = 0

	if !stopAfterTurn { // map already updated when stopping after turn
		// Assuming obstruction will only ever be in box in front (when stop after forward instruction)
		indx := getOneInFront(0)

		Map.Tiles[indx] = obstacleToValue(obstructionType)

		feed = "<br> <br> Obstruction identified: " + obstacleToName(obstructionType) + feed
	}

	feed = "<br> <br> Stopped due to obstruction <br> <br> Computing new shortest path " + feed
	fmt.Println("Stopped due to obstruction. Computing new shortest path.")

	if stopAutonomous == false {
		autonomousDrive(mqtt)
	} else {

		if err := mapAndDrive(mqtt, previousDestinationRow, previousDestinationCol, previousDestinationMode); err != nil {
			// Enough to log error => Error is handled manually by clicking again on map
			mqtt.getLogger().Error("server: map_general: stop: failed to compute new shortest path")
		}
	}
}

func updateMapWithObstructionWhileTurning(obstructionType string) {
	if stashedDriveInstruction.Instruction != "turnRight" && stashedDriveInstruction.Instruction != "turnLeft" {
		fmt.Println("server: map_general: updateMapWithObstructionWhileTurning fail, not currently turning")
		return
	}

	var changeInRotation int
	if stashedDriveInstruction.Instruction == "turnLeft" {
		changeInRotation = -stashedDriveInstruction.Value
	} else {
		changeInRotation = stashedDriveInstruction.Value
	}

	indx := getOneInFront(changeInRotation)

	Map.Tiles[indx] = obstacleToValue(obstructionType)
}

func obstacleToValue(obstacle string) int {
	if obstacle == "" { // No obstruction
		return 2
	} else if obstacle == "U" {
		return 5
	} else if obstacle == "B" {
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

	fmt.Println("server: map_general: obstacleToValue: unknown obstacle, returning default value 5")
	return 5
}

func obstacleToName(obstacle string) string {
	if obstacle == "" { // No obstruction
		return "No obstruction"
	} else if obstacle == "U" {
		return "Unknown obstruction"
	} else if obstacle == "B" {
		return "Blue ball"
	} else if obstacle == "R" {
		return "Red ball"
	} else if obstacle == "Y" {
		return "Yellow ball"
	} else if obstacle == "T" {
		return "Teal ball "
	} else if obstacle == "V" {
		return "Violet ball"
	}

	fmt.Println("server: map_general: obstacleToValue: unknown obstacle, returning default value 5")
	return "Unknown obstruction"
}

func getOneInFront(changeInRotation int) int {
	rotation := (Rover.Rotation + changeInRotation + 360) % 360

	if rotation == 0 {
		return (Rover.X + 1) + (Rover.Y * Map.Cols)
	} else if rotation == 180 {
		return (Rover.X - 1) + (Rover.Y * Map.Cols)
	} else if rotation == 90 {
		return Rover.X + ((Rover.Y + 1) * Map.Cols)
	} else if rotation == 270 {
		return Rover.X + ((Rover.Y - 1) * Map.Cols)
	}
	return 0
}
