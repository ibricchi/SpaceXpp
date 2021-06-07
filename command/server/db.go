package server

import (
	"context"
	"database/sql"

	"go.uber.org/zap"
)

type DB interface {
	saveMapName(ctx context.Context, name string) error
	insertMap(ctx context.Context, indx int, value int, mapID int) error
	retriveMap(ctx context.Context, mapID int) error
	retriveRover(ctx context.Context, mapID int) error
	getMapID(ctx context.Context, name string) (int, error)
	getLatestMapID(ctx context.Context) (int, error)
	saveRover(ctx context.Context, mapID int, roverIndex int) error
	storeInstruction(ctx context.Context, instruction string, value int) error
	resetInstructions(ctx context.Context, mapID int) error
	retriveInstruction(ctx context.Context, mapID int) error

	getLogger() *zap.Logger

	migrate(ctx context.Context) error
	TransactContext(ctx context.Context, f func(ctx context.Context, tx *sql.Tx) error) error
	Close() error
}
