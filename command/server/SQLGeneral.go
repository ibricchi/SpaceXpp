package server

import (
	"context"
	"database/sql"
	"fmt"
	"strconv"

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

	/* DATABASE Testing Area */
	fmt.Println("Stating db tests")
	statement, _ := db.Prepare("CREATE TABLE IF NOT EXISTS people (id INTEGER PRIMARY KEY, name TEXT)")
	statement.Exec()
	statement, _ = db.Prepare("INSERT INTO people (name) VALUES (?)")
	statement.Exec("Brad")

	fmt.Println("data in db, now querying")

	rows, _ := db.Query("SELECT id, name FROM people")
	var id int
	var name string
	for rows.Next() {
		rows.Scan(&id, &name)
		fmt.Println(strconv.Itoa(id) + ": " + name)
	}

	/*
		if err := s.migrate(ctx); err != nil {
			s.Close()
			return nil, fmt.Errorf("server: sqlite_db_general: failed to migrate SQLite database: %w", err)
		}
	*/
	return s, nil
}
