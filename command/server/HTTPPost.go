package server

import (
	"encoding/json"
	"fmt"
	"net/http"
)

type coordinates struct {
	X    int `json:"x"`
	Y    int `json:"y"`
	Mode int `json:"mode"`
}

func (h *HttpServer) speed(w http.ResponseWriter, r *http.Request) {
	decoder := json.NewDecoder(r.Body)
	defer r.Body.Close()

	var t int
	if err := decoder.Decode(&t); err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
	}

	// Check for correct format

	if err := h.db.insertData(true, t); err != nil {
		http.Error(w, "Error: Failed to insert data in DB", http.StatusInternalServerError)
	}

	w.WriteHeader(http.StatusOK)

	fmt.Println("Recived speed: ", t)

}

func (h *HttpServer) driveD(w http.ResponseWriter, r *http.Request) {
	decoder := json.NewDecoder(r.Body)
	defer r.Body.Close()

	var t int
	if err := decoder.Decode(&t); err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
	}

	if err := h.db.insertData(true, t); err != nil {
		http.Error(w, "Error: Failed to insert data in DB", http.StatusInternalServerError)
	}

	w.WriteHeader(http.StatusOK)

	instruction := []driveInstruction{}

	instruction = append(instruction, driveInstruction{
		instruction: "forward",
		value:       t,
	})

	h.mqtt.publishDriveInstructionSequence(instruction)

	//updateMap(instruction[0])

}

func (h *HttpServer) driveA(w http.ResponseWriter, r *http.Request) {
	decoder := json.NewDecoder(r.Body)
	defer r.Body.Close()

	var t int
	if err := decoder.Decode(&t); err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
	}

	// Check for correct format

	if err := h.db.insertData(true, t); err != nil {
		http.Error(w, "Error: Failed to insert data in DB", http.StatusInternalServerError)
	}

	w.WriteHeader(http.StatusOK)

	fmt.Println("Recived drive angle: ", t)
	// Send data to hardware

	c := "turnRight"
	if t < 0 {
		c = "turnLeft"
	}

	instruction := []driveInstruction{}

	instruction = append(instruction, driveInstruction{
		instruction: c,
		value:       Abs(t),
	})

	h.mqtt.publishDriveInstructionSequence(instruction)

	//updateMap(instruction[0])

}

func (h *HttpServer) targetCoords(w http.ResponseWriter, r *http.Request) {
	decoder := json.NewDecoder(r.Body)
	defer r.Body.Close()

	var targetCoords coordinates
	if err := decoder.Decode(&targetCoords); err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
	}

	w.WriteHeader(http.StatusOK)

	//stop(1, 0)

	if err := h.mapAndDrive(targetCoords.X, targetCoords.Y, targetCoords.Mode); err != nil {
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

func (h *HttpServer) resetMap(w http.ResponseWriter, r *http.Request) {
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

}
