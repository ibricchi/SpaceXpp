package server

import (
	"context"
	"net/http"

	"github.com/go-chi/chi"
	"github.com/go-chi/chi/middleware"
	"github.com/go-chi/cors"
)

func allowOriginFunc(r *http.Request, origin string) bool {
	// Allow all without checking origin
	return true
}

func (h *HttpServer) routes(ctx context.Context) error {

	// General middleware
	h.router.Use(cors.Handler(cors.Options{
		AllowOriginFunc:  allowOriginFunc,
		AllowedMethods:   []string{"GET", "POST", "PUT", "DELETE", "OPTIONS"},
		AllowedHeaders:   []string{"X-PINGOTHER", "Accept", "Authorization", "Content-Type", "X-CSRF-Token"},
		ExposedHeaders:   []string{"Link"},
		AllowCredentials: true,
	}))
	h.router.Use(middleware.Logger)

	// Public routes
	h.router.Group(func(r chi.Router) {
		// Get
		r.Get("/connect", connect)
		r.Get("/battery", battery)
		r.Get("/check", check)

	})
	return nil
}
