package server

import (
	"context"
	"database/sql"
	"fmt"
)

type DB interface {
	saveMapName(ctx context.Context, name string) error
	insertMap(ctx context.Context, indx int, value int, mapID int) error
	retriveMap(ctx context.Context, mapID int) error
	getMapID(ctx context.Context, name string) (int, error)
	Close() error
}

func (s *SQLiteDB) saveMapName(ctx context.Context, name string) error {
	if err := s.TransactContext(ctx, func(ctx context.Context, tx *sql.Tx) error {
		if _, err := tx.ExecContext(ctx, `
			INSERT INTO maps (name)
			VALUES (:name)
		`,
			sql.Named("name", name),
		); err != nil {
			fmt.Println("not inserted:", name)
			return fmt.Errorf("server: sqlite_db_insert: failed to insert map name data into db: %w", err)
		}
		fmt.Println("inserted:", name)

		return nil
	}); err != nil {
		return fmt.Errorf("server: sqlite_db_insert: insert map name transaction failed: %w", err)
	}
	return nil
}

func (s *SQLiteDB) insertMap(ctx context.Context, indx int, value int, mapID int) error {
	if err := s.TransactContext(ctx, func(ctx context.Context, tx *sql.Tx) error {
		if _, err := tx.ExecContext(ctx, `
			INSERT INTO tiles (name)
			VALUES (:name)
		`,
			sql.Named("name", "test33"),
		); err != nil {
			fmt.Println("not inserted:", "test33")
			return fmt.Errorf("server: sqlite_db_insert: failed to insert map name data into db: %w", err)
		}
		fmt.Println("inserted:", "test33")

		return nil
	}); err != nil {
		return fmt.Errorf("server: sqlite_db_insert: insert map name transaction failed: %w", err)
	}
	return nil
}

func (s *SQLiteDB) retriveMap(ctx context.Context, mapID int) error {

	if err := s.TransactContext(ctx, func(ctx context.Context, tx *sql.Tx) error {
		rows, err := tx.QueryContext(ctx, `
			SELECT tileID, value 
			FROM tiles 
			WHERE mapID = mID
			`,
			sql.Named("mID", mapID),
		)
		if err != nil {
			return fmt.Errorf("server: sqlite_db_retrieve: failed to retrieve data from tiles rows: %w", err)
		}
		defer rows.Close()

		for rows.Next() {
			var tileID, value int
			if err := rows.Scan(
				&tileID,
				&value,
			); err != nil {
				return fmt.Errorf("server: sqlite_db_retrieve: failed to scan tiles row: %w", err)
			}
			fmt.Println("map:", tileID, value)
			dbMap.Tiles[tileID] = value
		}
		if err := rows.Err(); err != nil {
			return fmt.Errorf("server: sqlite_db_retrieve: failed to scan last mapID row: %w", err)
		}

		return nil
	}); err != nil {
		return fmt.Errorf("server: sqlite_db_retrieve: mapID transaction failed: %w", err)
	}

	return nil
}
func (s *SQLiteDB) getMapID(ctx context.Context, name string) (int, error) {

	var id int
	if err := s.TransactContext(ctx, func(ctx context.Context, tx *sql.Tx) error {
		if err := tx.QueryRowContext(ctx, `
			SELECT mapID
			FROM maps
			WHERE name = :mapName
		`,
			sql.Named("mapName", name),
		).Scan(&id); err != nil {
			return fmt.Errorf("server: sqlite_db_retrieve: failed to find id row: %w", err)
		}
		fmt.Println("id:", id)

		return nil
	}); err != nil {
		return -1, fmt.Errorf("server: sqlite_db_retrieve: get map id transaction failed: %w", err)
	}

	return id, nil
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
