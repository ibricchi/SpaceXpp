package server

import (
	"encoding/json"
	"fmt"
	"net/http"
	"strconv"
)

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

	fmt.Println("Recived drive distance: ", t)

	// Send data to hardware
	h.mqtt.publish("/drive/distance", strconv.Itoa(t), 0)
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

}
