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
		return nil, fmt.Errorf("server: SQLGeneral: failed to open sqlite db: %w", err)
	}

	if err := db.PingContext(ctx); err != nil {
		db.Close()
		return nil, fmt.Errorf("server: SQLGeneral: sqlite db down: %w", err)
	}

	s := &SQLiteDB{
		db:     db,
		logger: logger,
	}

	if err := s.migrate(ctx); err != nil {
		s.Close()
		return nil, fmt.Errorf("server: SQLGeneral: failed to migrate SQLite database: %w", err)
	}

	return s, nil
}

func (s *SQLiteDB) migrate(ctx context.Context) error {
	if err := s.TransactContext(ctx, func(ctx context.Context, tx *sql.Tx) error {

		if _, err := tx.ExecContext(ctx, `
				CREATE TABLE IF NOT EXISTS maps (
					mapID INTEGER NOT NULL PRIMARY KEY,
					name STRING	NOT NULL
				)
			`); err != nil {
			return fmt.Errorf("sqlite failed to create maps table: %w", err)
		}

		if _, err := tx.ExecContext(ctx, `
			CREATE TABLE IF NOT EXISTS tiles (
				tileID INTEGER NOT NULL PRIMARY KEY,
				indx INTEGER NOT NULL,
				mapID INTEGER NOT NULL,
				value INTEGER NOT NULL,
				FOREIGN KEY(mapID) REFERENCES maps(mapID)	
			)
				`); err != nil {
			return fmt.Errorf("sqlite failed to create tiles table: %w", err)
		}

		if _, err := tx.ExecContext(ctx, `
			CREATE TABLE IF NOT EXISTS rover (
				mapID INTEGER NOT NULL PRIMARY KEY,
				indx INTEGER NOT NULL,
				rotation INTEGER NOT NULL,
				FOREIGN KEY(mapID) REFERENCES maps(mapID)		
			)
				`); err != nil {
			return fmt.Errorf("sqlite failed to create rover table: %w", err)
		}

		if _, err := tx.ExecContext(ctx, `
			CREATE TABLE IF NOT EXISTS instructions (
				instructionID INTEGER NOT NULL PRIMARY KEY,
				mapID INTEGER NOT NULL,
				instruction STRING NOT NULL,
				value INTEGER NOT NULL,
				FOREIGN KEY(mapID) REFERENCES maps(mapID)	
			)
				`); err != nil {
			return fmt.Errorf("sqlite failed to create instruction table: %w", err)
		}

		if _, err := tx.ExecContext(ctx, `
			CREATE TABLE IF NOT EXISTS credentials (
				id INTEGER NOT NULL PRIMARY KEY,
				username TEXT NOT NULL,
				password TEXT NOT NULL
			)
		`); err != nil {
			return fmt.Errorf("sqlite failed to create creds table: %w", err)
		}

		return nil
	}); err != nil {
		return fmt.Errorf("server: SQLGeneral: migrate transaction failed: %w", err)
	}

	return nil
}

func (s *SQLiteDB) TransactContext(ctx context.Context, f func(ctx context.Context, tx *sql.Tx) error) (err error) {
	tx, err := s.db.BeginTx(ctx, nil)
	if err != nil {
		return fmt.Errorf("server: SQLGeneral: failed to begin a transaction: %w", err)
	}

	defer func() {
		if err != nil {
			if err := tx.Rollback(); err != nil {
				s.logger.Error("server: SQLGeneral: transaction rollback failed")
			}
			return
		}

		err = tx.Commit()
	}()

	return f(ctx, tx)
}
