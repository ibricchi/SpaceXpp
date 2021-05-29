package server

import (
	"encoding/json"
	"fmt"
	"net/http"
	"strconv"
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

	var a []driveInstruction
	a[0].instruction = "forward"
	a[0].value = t
	h.mqtt.publishDriveInstructionSequence(a)

	updateMap(a[0])

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

	h.mqtt.publish("/drive/angle", strconv.Itoa(t), 0)

	var a driveInstruction
	a.instruction = "forward"
	a.value = 1
	fmt.Println("forward instruction")
	updateMap(a)

	var c []driveInstruction

	if t > 0 {
		c[0].instruction = "turnRight"
	} else {
		c[0].instruction = "turnLeft"
	}

	c[0].value = Abs(t)

	h.mqtt.publishDriveInstructionSequence(c)

	updateMap(c[0])

}

func (h *HttpServer) targetCoords(w http.ResponseWriter, r *http.Request) {
	decoder := json.NewDecoder(r.Body)
	defer r.Body.Close()

	var targetCoords coordinates
	if err := decoder.Decode(&targetCoords); err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
	}

	w.WriteHeader(http.StatusOK)

	stop(1, 0)

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
