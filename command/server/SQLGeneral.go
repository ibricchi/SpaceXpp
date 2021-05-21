package server

import (
	"context"
	"database/sql"
	"fmt"

	_ "github.com/mattn/go-sqlite3"
	"go.uber.org/zap"
)

type SQLiteDB struct {
	db     *sql.DB
	logger *zap.Logger
}

func OpenSQLiteDB(ctx context.Context, logger *zap.Logger, dsn string) (*SQLiteDB, error) {
	db, err := sql.Open("sqlite3", dsn)
	if err != nil {
		return nil, fmt.Errorf("server: sqlite_db_general: failed to open sqlite db: %w", err)
	}

	if err := db.PingContext(ctx); err != nil {
		db.Close()
		return nil, fmt.Errorf("server: sqlite_db_general: sqlite db down: %w", err)
	}

	s := &SQLiteDB{
		db:     db,
		logger: logger,
	}

	/*
		if err := s.migrate(ctx); err != nil {
			s.Close()
			return nil, fmt.Errorf("server: sqlite_db_general: failed to migrate SQLite database: %w", err)
		}
	*/
	return s, nil
}
