package server

import (
	"fmt"
	"net/http"
)

func speed(w http.ResponseWriter, req *http.Request) {
	fmt.Println("speed")
}
