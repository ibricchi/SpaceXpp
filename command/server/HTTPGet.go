package server

import (
	"encoding/json"
	"net/http"
)

func connect(w http.ResponseWriter, req *http.Request) {
	status := "connected"
	json.NewEncoder(w).Encode(status)

}

func battery(w http.ResponseWriter, req *http.Request) {
	level := 69
	json.NewEncoder(w).Encode(level)

}
