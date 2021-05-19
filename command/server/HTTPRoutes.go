package server

import (
	"context"

	"github.com/go-chi/chi"
)

func (h *HttpServer) routes(ctx context.Context) error {

	// Public routes
	h.router.Group(func(r chi.Router) {
		// Get
		r.Get("/connect", connect)
		r.Get("/battery", battery)
	})
	return nil
}
