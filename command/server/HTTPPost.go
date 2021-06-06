package server

import (
	"context"
	"encoding/json"
	"fmt"
	"net/http"
)

type coordinates struct {
	X    int `json:"x"`
	Y    int `json:"y"`
	Mode int `json:"mode"`
}

func (h *HttpServer) driveD(w http.ResponseWriter, r *http.Request) {
	decoder := json.NewDecoder(r.Body)
	defer r.Body.Close()

	var t int
	if err := decoder.Decode(&t); err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
	}

	w.WriteHeader(http.StatusOK)

	instruction := []driveInstruction{}

	instruction = append(instruction, driveInstruction{
		Instruction: "forward",
		Value:       t,
	})

	//h.mqtt.publishDriveInstructionSequence(instruction)

	//updateMap(instruction[0])

}
func (h *HttpServer) driveA(ctx context.Context) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		decoder := json.NewDecoder(r.Body)
		defer r.Body.Close()

		var t int
		if err := decoder.Decode(&t); err != nil {
			http.Error(w, err.Error(), http.StatusBadRequest)
		}

		// Check for correct format

		w.WriteHeader(http.StatusOK)

		fmt.Println("Recived drive angle: ", t)
		// Send data to hardware

		c := "turnRight"
		if t < 0 {
			c = "turnLeft"
		}

		instruction := []driveInstruction{}

		instruction = append(instruction, driveInstruction{
			Instruction: c,
			Value:       Abs(t),
		})

		//	h.mqtt.publishDriveInstructionSequence(instruction)

		fmt.Println("updating map & inserting instructions ")

		//updateMap(instruction[0], ctx)

		//fmt.Println("attempting to store in db")

		//h.insertInstruction(ctx, instruction[0])

		/*
			currentEnergy.StateOfCharge = 12
			currentEnergy.StateOfHealth = 2
			currentEnergy.ErrorInCells = 1
		*/
	}
}

func (h *HttpServer) targetCoords(w http.ResponseWriter, r *http.Request) {
	decoder := json.NewDecoder(r.Body)
	defer r.Body.Close()

	var targetCoords coordinates
	if err := decoder.Decode(&targetCoords); err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
	}

	w.WriteHeader(http.StatusOK)

	previousDestinationRow = targetCoords.X
	previousDestinationCol = targetCoords.Y
	previousDestinationMode = targetCoords.Mode

	if err := mapAndDrive(h.mqtt, targetCoords.X, targetCoords.Y, targetCoords.Mode); err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
	}
}

func Abs(x int) int {
	if x < 0 {
		return -1 * x
	} else {
		return x
	}
}
func (h *HttpServer) resetMap(ctx context.Context) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		Map.Tiles = []int{
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
			3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3}

		Rover.X = 5
		Rover.Y = 5
		Rover.Rotation = 0

		mapID, err := h.db.getLatestMapID(ctx)
		if err != nil {
			fmt.Println("no mapID : ", mapID)
			fmt.Println("Error: couldnt get latest map ID")
		}

		var empty []driveInstruction
		dbMap.Instructions = empty

		h.db.resetInstructions(ctx, (mapID + 1))

	}
}
func (h *HttpServer) requestMap(ctx context.Context) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {

		// Aquiring map name

		decoder := json.NewDecoder(r.Body)
		defer r.Body.Close()

		var name string
		if err := decoder.Decode(&name); err != nil {
			http.Error(w, err.Error(), http.StatusBadRequest)
		}

		w.WriteHeader(http.StatusOK)

		fmt.Println("map name:", name)

		// map is quered using name to get id

		mapID, err := h.db.getMapID(ctx, name)
		if err != nil {
			http.Error(w, err.Error(), http.StatusBadRequest)
		}

		fmt.Println("map ID:", mapID)

		// map is built and stored in dbmap
		h.db.retriveMap(ctx, mapID)

		// Rover built and stored in dbmap
		h.db.retriveRover(ctx, mapID)

		// Instructions Built and stored in dbmap

		h.db.retriveInstruction(ctx, mapID)

	}
}

func (h *HttpServer) save(ctx context.Context) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		decoder := json.NewDecoder(r.Body)
		defer r.Body.Close()

		var name string
		if err := decoder.Decode(&name); err != nil {
			http.Error(w, err.Error(), http.StatusBadRequest)
		}

		h.db.saveMapName(ctx, name)

		mapID, err := h.db.getMapID(ctx, name)
		if err != nil {
			http.Error(w, err.Error(), http.StatusBadRequest)
		}

		h.convertAndInsert(ctx, mapID)

		roverIndex := Rover.X + (Rover.Y * Map.Cols)
		fmt.Println("Rover index =", roverIndex)
		h.db.saveRover(ctx, mapID, roverIndex)

		w.WriteHeader(http.StatusOK)

	}
}
