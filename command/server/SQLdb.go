package server

import (
	"context"
	"database/sql"
	"fmt"
)

type DB interface {
	insertData(ctx context.Context, name string) error
	retriveData(ctx context.Context) (string, error)
	Close() error
}

func (s *SQLiteDB) insertData(ctx context.Context, name string) error {
	if err := s.TransactContext(ctx, func(ctx context.Context, tx *sql.Tx) error {
		if _, err := tx.ExecContext(ctx, `
			INSERT INTO maps (name)
			VALUES (:name)
		`,
			sql.Named("name", name),
		); err != nil {
			return fmt.Errorf("server: sqlite_db_insert: failed to insert test data into db: %w", err)
		}

		return nil
	}); err != nil {
		return fmt.Errorf("server: sqlite_db_insert: insertTestData transaction failed: %w", err)
	}
	return nil
}

func (s *SQLiteDB) retriveData(ctx context.Context) (string, error) {

	var name string
	if err := s.TransactContext(ctx, func(ctx context.Context, tx *sql.Tx) error {
		rows, err := tx.QueryContext(ctx, `
			SELECT name
			FROM maps
		`)
		if err != nil {
			return fmt.Errorf("server: sqlite_db_retrieve: failed to retrieve creds rows: %w", err)
		}
		defer rows.Close()

		for rows.Next() {
			var name string
			if err := rows.Scan(
				&name,
			); err != nil {
				return fmt.Errorf("server: sqlite_db_retrieve: failed to scan creds row: %w", err)
			}
			fmt.Println("name:", name)
		}

		if err := rows.Err(); err != nil {
			return fmt.Errorf("server: sqlite_db_retrieve: failed to scan last creds row: %w", err)
		}

		return nil
	}); err != nil {
		return "empty", fmt.Errorf("server: sqlite_db_retrieve: getCreds transaction failed: %w", err)
	}

	return name, nil
}

func (s *SQLiteDB) Close() error {
	if err := s.db.Close(); err != nil {
		return fmt.Errorf("server: sqlite_db_general: failed to close sqlite db: %w", err)
	}
	return nil
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/* DATABASE Testing Area /
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
/* end of testing area */
//////////////////////////////////////////////////////////////////////////////////////////////////////////
