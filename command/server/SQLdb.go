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
	retriveData(ctx context.Context) (int, error)
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
			return fmt.Errorf("server: sqlite_db_insert: failed to insert map name data into db: %w", err)
		}

		return nil
	}); err != nil {
		return fmt.Errorf("server: sqlite_db_insert: insert map name transaction failed: %w", err)
	}
	return nil
}

func (s *SQLiteDB) insertMap(ctx context.Context, indx int, value int, mapID int) error {
	if err := s.TransactContext(ctx, func(ctx context.Context, tx *sql.Tx) error {
		if _, err := tx.ExecContext(ctx, `
			INSERT INTO tiles (tileID, mapID, value)
			VALUES (:tileID, :mapID, :value)
		`,
			sql.Named("tileID", indx),
			sql.Named("mapID", mapID),
			sql.Named("value", value),
		); err != nil {
			return fmt.Errorf("server: sqlite_db_insert: failed to insert tiles into db: %w", err)
		}

		return nil
	}); err != nil {
		return fmt.Errorf("server: sqlite_db_insert: insertTestData transaction failed: %w", err)
	}
	return nil
}

func (s *SQLiteDB) getMapID(ctx context.Context, name string) (int, error) {

	fmt.Println("getting map ID")
	var id int
	if err := s.TransactContext(ctx, func(ctx context.Context, tx *sql.Tx) error {
		rows, err := tx.QueryContext(ctx, `
			SELECT mapID 
			FROM maps 
			WHERE name = `+name,
		)
		if err != nil {
			return fmt.Errorf("server: sqlite_db_retrieve: failed to retrieve nameID rows: %w", err)

		}
		defer rows.Close()

		for rows.Next() {
			fmt.Println("pleaseeee")
			if err := rows.Scan(
				&id,
			); err != nil {
				return fmt.Errorf("server: sqlite_db_retrieve: failed to scan mapID row: %w", err)
			}
			fmt.Println("map id:", id)
		}

		if err := rows.Err(); err != nil {
			return fmt.Errorf("server: sqlite_db_retrieve: failed to scan last mapID row: %w", err)
		}

		return nil
	}); err != nil {
		return -1, fmt.Errorf("server: sqlite_db_retrieve: mapID transaction failed: %w", err)
	}

	return id, nil
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

func (s *SQLiteDB) retriveData(ctx context.Context) (int, error) {

	var name int
	if err := s.TransactContext(ctx, func(ctx context.Context, tx *sql.Tx) error {
		rows, err := tx.QueryContext(ctx, `
			SELECT mapID
			FROM maps
			WHERE name = :mapName
			LIMIT 1
		`,
			sql.Named("mapName", "test5"),
		)
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
		return -1, fmt.Errorf("server: sqlite_db_retrieve: getCreds transaction failed: %w", err)
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
