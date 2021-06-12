/*
   Written by Bradley Stanley-Clamp (bradley.stanley-clamp19@imperial.ac.uk) and Nicholas Pfaff (nicholas.pfaff19@imperial.ac.uk), 2021 - SpaceX++ EEE/EIE 2nd year group project, Imperial College London
*/

package server

import (
	"context"
	"database/sql"

	"go.uber.org/zap"
)

type DB interface {
	getLogger() *zap.Logger

	saveMapName(ctx context.Context, name string) error
	saveRover(ctx context.Context, mapID int, roverIndex int) error
	insertMap(ctx context.Context, tiles []int, mapID int) error
	retriveMap(ctx context.Context, mapID int) error
	retriveRover(ctx context.Context, mapID int) error
	getMapID(ctx context.Context, name string) (int, error)
	getLatestMapID(ctx context.Context) (int, error)
	storeInstruction(ctx context.Context, instruction string, value int) error
	retriveInstruction(ctx context.Context, mapID int) error
	resetInstructions(ctx context.Context, mapID int) error
	insertCredentials(ctx context.Context, credential credential) error
	getCredentials(ctx context.Context) (map[string]string, error)

	migrate(ctx context.Context) error
	TransactContext(ctx context.Context, f func(ctx context.Context, tx *sql.Tx) error) (err error)
	Close() error
}
