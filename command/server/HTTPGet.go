package server

import (
	"encoding/json"
	"net/http"
)

type staticTestData struct {
	Info string `json:"info"`
	Data []int  `json:"data"`
}

type mAp struct {
	Info string `json:"info"`
	Cols int    `json:"cols"`
	Rows int    `json:"rows"`
}

func (h *HttpServer) connect(w http.ResponseWriter, req *http.Request) {
	status := 1
	w.Header().Set("Content-Type", "application/json; charset=UTF-8")

	if err := json.NewEncoder(w).Encode(status); err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
	}

	w.WriteHeader(http.StatusOK)

}

func (h *HttpServer) battery(w http.ResponseWriter, req *http.Request) {

	err, _, level := h.db.retriveData()
	if err != nil {
		//return error
	}

	w.Header().Set("Content-Type", "application/json; charset=UTF-8")

	if err := json.NewEncoder(w).Encode(level); err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
	}

	w.WriteHeader(http.StatusOK)
}

func check(w http.ResponseWriter, req *http.Request) {

	w.Header().Set("Content-Type", "application/json; charset=UTF-8")

	data := staticTestData{
		Info: "Some static test data",
		Data: []int{
			1, 2, 3, 4, 5,
		},
	}
	if err := json.NewEncoder(w).Encode(data); err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
	}

	w.WriteHeader(http.StatusOK)
}

func (h *HttpServer) getMap(w http.ResponseWriter, req *http.Request) {

	w.Header().Set("Content-Type", "application/json; charset=UTF-8")

	data := mAp{
		Info: "map",
		Cols: 12,
		Rows: 12,
	}
	if err := json.NewEncoder(w).Encode(data); err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
	}

	w.WriteHeader(http.StatusOK)
}
