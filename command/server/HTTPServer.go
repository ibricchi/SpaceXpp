package server

import (
	"context"
	"fmt"
	"net/http"

	"github.com/go-chi/chi"
	"go.uber.org/zap"
)

type HttpServer struct {
	db     DB
	router *chi.Mux
	logger *zap.Logger
}

func OpenHttpServer(ctx context.Context, logger *zap.Logger, router *chi.Mux, db *SQLiteDB) *HttpServer {
	h := &HttpServer{
		db:     db,
		router: router,
		logger: logger,
	}

	return h
}

func hello(w http.ResponseWriter, req *http.Request) {
	fmt.Fprintf(w, "hello\n")
}

func (h *HttpServer) Serve(ctx context.Context, port string) error {
	if err := h.routes(ctx); err != nil {
		return fmt.Errorf("server: http_server: routes failed: %w", err)
	}

	http.HandleFunc("/hello", hello)

	portStr := ":" + port

	fmt.Fprint("listennninnnggg")

	if err := http.ListenAndServe(portStr, nil); err != nil {
		return fmt.Errorf("server: http_server: http.ListenAndServe failed: %w", err)
	}
	return nil
}
