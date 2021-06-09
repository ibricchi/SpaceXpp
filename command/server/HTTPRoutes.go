package server

import (
	"context"
	"fmt"

	"github.com/go-chi/chi"
	"github.com/go-chi/chi/middleware"
	"github.com/go-chi/cors"
)

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

	// Credentials from database
	credentials, err := h.db.getCredentials(ctx)
	if err != nil {
		return fmt.Errorf("server: routes: failed to get credentials: %w", err)
	}

	// Public routes
	h.router.Group(func(r chi.Router) {
		r.Get("/isAuthorised", h.getIsAuthorised(credentials))
	})

	// Private routes
	h.router.Group(func(r chi.Router) {
		r.Use(h.basicAuth("SpaceXpp-Server", credentials))

		// Get
		r.Get("/connect", h.connect)
		r.Get("/battery", h.battery)
		r.Get("/check", check)
		r.Get("/feed", h.getFeed(ctx))
		r.Get("/map/getMap", h.updateWebMap)
		r.Get("/map/getRover", h.updateRover)
		r.Get("/map/history/load", h.loadMap(ctx))
		r.Get("/energy/values", h.getEnergyStatus)
		r.Get("/map/history/names", h.getMapNames(ctx))

		// Post
		r.Post("/drive/distance", h.driveD)
		r.Post("/drive/angle", h.driveA(ctx))
		r.Post("/map/targetCoords", h.targetCoords)
		r.Post("/map/reset", h.resetMap(ctx))
		r.Post("/map/history/request", h.requestMap(ctx))
		r.Post("/map/history/save", h.save(ctx))
		r.Post("/map/stopAutonomous", h.stopAutonom)

	})

	return nil
}
