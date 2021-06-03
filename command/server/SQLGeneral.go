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

	if err := s.migrate(ctx); err != nil {
		s.Close()
		return nil, fmt.Errorf("server: sqlite_db_general: failed to migrate SQLite database: %w", err)
	}

	return s, nil
}

func (s *SQLiteDB) migrate(ctx context.Context) error {

	if _, err := s.db.ExecContext(ctx, `
			CREATE TABLE IF NOT EXISTS maps (
				mapID INTEGER NOT NULL PRIMARY KEY,
				name STRING	
			)
		`); err != nil {
		return fmt.Errorf("sqlite failed to create maps table: %w", err)
	}

	if _, err := s.db.ExecContext(ctx, `
		CREATE TABLE IF NOT EXISTS tiles (
			tileID INTEGER NOT NULL PRIMARY KEY,
			mapID INTEGER,
			value INTEGER,
			FOREIGN KEY(mapID) REFERENCES Departments(maps)
		)
			`); err != nil {
		return fmt.Errorf("sqlite failed to create tiles table: %w", err)
	}

	return nil

}

func (s *SQLiteDB) TransactContext(ctx context.Context, f func(ctx context.Context, tx *sql.Tx) error) (err error) {
	tx, err := s.db.BeginTx(ctx, nil)
	if err != nil {
		return fmt.Errorf("buna: sqlite_db_general: failed to begin a transaction: %w", err)
	}

	defer func() {
		if err != nil {
			if err := tx.Rollback(); err != nil {
				s.logger.Error("buna: sqlite_db_general: transaction rollback failed")
			}
			return
		}

		err = tx.Commit()
	}()

	return f(ctx, tx)
}
