package server

import (
	"encoding/json"
	"fmt"
	"net/http"
)

func speed(w http.ResponseWriter, r *http.Request) {
	decoder := json.NewDecoder(r.Body)
	defer r.Body.Close()

	var t int
	err := decoder.Decode(&t)

	if err != nil {
		panic(err)
	}

	fmt.Println(t)

}
