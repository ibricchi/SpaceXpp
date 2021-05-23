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
	err := decoder.Decode(&t)

	if err != nil {
		panic(err)
	}

	err = h.db.insertData(true, t)
	if err != nil {
		panic(err)
	}

	fmt.Println(t)

}

func (h *HttpServer) driveD(w http.ResponseWriter, r *http.Request) {
	decoder := json.NewDecoder(r.Body)
	defer r.Body.Close()

	var t int
	err := decoder.Decode(&t)

	if err != nil {
		panic(err)
	}

	fmt.Println(t)
	// Send data to hardware
	h.mqtt.publish("/drive/distance", strconv.Itoa(t), 0)
}

func (h *HttpServer) driveA(w http.ResponseWriter, r *http.Request) {
	decoder := json.NewDecoder(r.Body)
	defer r.Body.Close()

	var t int
	err := decoder.Decode(&t)

	if err != nil {
		panic(err)
	}

	fmt.Println(t)

	// Send data to hardware

	h.mqtt.publish("/drive/angle", strconv.Itoa(t), 0)

}
