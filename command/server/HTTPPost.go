package server

import (
	"fmt"
	"net/http"
)

func speed(w http.ResponseWriter, req *http.Request) {
	fmt.Fprintf(w, "speed: %v", req.Body)
}
