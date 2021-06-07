package server

import (
	"context"
	"database/sql"
	"fmt"
)

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
		return nil
	}); err != nil {
		return fmt.Errorf("server: sqlite_db_insert: insert map name transaction failed: %w", err)
	}
	return nil
}

func (s *SQLiteDB) saveRover(ctx context.Context, mapID int, roverIndex int) error {
	if err := s.TransactContext(ctx, func(ctx context.Context, tx *sql.Tx) error {
		if _, err := tx.ExecContext(ctx, `
			INSERT INTO rover (mapID, indx, rotation)
			VALUES (:mapID, :indx, :rotation)
		`,
			sql.Named("mapID", mapID),
			sql.Named("indx", roverIndex),
			sql.Named("rotation", Rover.Rotation),
		); err != nil {
			fmt.Println("rover not inserted")
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
			INSERT INTO tiles (indx, mapID, value)
			VALUES (:indx, :mapID, :value )
		`,
			sql.Named("indx", indx),
			sql.Named("mapID", mapID),
			sql.Named("value", value),
		); err != nil {
			fmt.Println("not inserted:", indx, value, mapID)
			return fmt.Errorf("server: sqlite_db_insert: failed to insert tiles into db: %w", err)
		}
		return nil
	}); err != nil {
		return fmt.Errorf("server: sqlite_db_insert: insertTestData transaction failed: %w", err)
	}

	return nil
}

func (s *SQLiteDB) retriveMap(ctx context.Context, mapID int) error {

	if err := s.TransactContext(ctx, func(ctx context.Context, tx *sql.Tx) error {
		rows, err := tx.QueryContext(ctx, `
			SELECT indx, value 
			FROM tiles 
			WHERE mapID = :mID
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

func (s *SQLiteDB) retriveRover(ctx context.Context, mapID int) error {

	var indx, rotation int
	if err := s.TransactContext(ctx, func(ctx context.Context, tx *sql.Tx) error {
		if err := tx.QueryRowContext(ctx, `
			SELECT indx, rotation
			FROM rover
			WHERE mapID = :mID
		`,
			sql.Named("mID", mapID),
		).Scan(&indx, &rotation); err != nil {
			return fmt.Errorf("server: sqlite_db_retrieve: failed to find id row: %w", err)
		}

		fmt.Println("rover index: ", indx, "rotation: ", rotation)
		dbMap.RoverRotation = rotation
		dbMap.RoverIndx = indx
		return nil
	}); err != nil {
		return fmt.Errorf("server: sqlite_db_retrieve: get map id transaction failed: %w", err)
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
		return nil
	}); err != nil {
		return -1, fmt.Errorf("server: sqlite_db_retrieve: get map id transaction failed: %w", err)
	}

	return id, nil
}

func (s *SQLiteDB) getLatestMapID(ctx context.Context) (int, error) {

	var id int
	if err := s.TransactContext(ctx, func(ctx context.Context, tx *sql.Tx) error {
		if err := tx.QueryRowContext(ctx, `
			SELECT max(mapID)
			FROM maps
		`,
		).Scan(&id); err != nil {
			id = 0
			fmt.Println("couldnt get latest map id, setting mapID to 0 (so next is 1)")
			return fmt.Errorf("server: sqlite_db_retrieve: failed to find id row: %w", err)
		}

		return nil
	}); err != nil {
		return -1, fmt.Errorf("server: sqlite_db_retrieve: get map id transaction failed: %w", err)
	}

	return id, nil
}

func (s *SQLiteDB) storeInstruction(ctx context.Context, instruction string, value int) error {

	fmt.Println("storing instruction: inside function")

	mapID, err := s.getLatestMapID(ctx)
	if err != nil {
		fmt.Println("no mapID : ", mapID)
		fmt.Println("Error: couldnt get latest map ID")
	}

	fmt.Println("storing instruction: map id:", mapID)

	if err := s.TransactContext(ctx, func(ctx context.Context, tx *sql.Tx) error {
		if _, err := tx.ExecContext(ctx, `
			INSERT INTO instructions (mapID, instruction, value)
			VALUES (:mapID, :instruction, :value )
		`,
			sql.Named("mapID", (mapID+1)),
			sql.Named("instruction", instruction),
			sql.Named("value", value),
		); err != nil {
			fmt.Println("not inserted:", mapID, instruction, value)
			return fmt.Errorf("server: sqlite_db_insert: failed to insert tiles into db: %w", err)
		}
		fmt.Println("inserted:", mapID, instruction, value)

		return nil
	}); err != nil {
		return fmt.Errorf("server: sqlite_db_insert: insertTestData transaction failed: %w", err)
	}

	return nil
}

func (s *SQLiteDB) retriveInstruction(ctx context.Context, mapID int) error {

	var instr string
	var val int

	if err := s.TransactContext(ctx, func(ctx context.Context, tx *sql.Tx) error {
		rows, err := tx.QueryContext(ctx, `
			SELECT instruction, value 
			FROM instructions
			WHERE mapID = :mID
			ORDER BY instructionID 
			`,
			sql.Named("mID", mapID),
		)
		if err != nil {
			fmt.Println("did not work extracting instructions")
			return fmt.Errorf("server: sqlite_db_retrieve: failed to retrieve data from tiles rows: %w", err)
		}
		defer rows.Close()
		for rows.Next() {
			if err := rows.Scan(
				&instr,
				&val,
			); err != nil {
				return fmt.Errorf("server: sqlite_db_retrieve: failed to scan tiles row: %w", err)
			}

			dbMap.Instructions = append(dbMap.Instructions, driveInstruction{
				Instruction: instr,
				Value:       val,
			})

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

func (s *SQLiteDB) resetInstructions(ctx context.Context, mapID int) error {

	if err := s.TransactContext(ctx, func(ctx context.Context, tx *sql.Tx) error {
		if _, err := tx.ExecContext(ctx, `
			DELETE FROM instructions
			WHERE mapID = :mapID
		`,
			sql.Named("mapID", mapID),
		); err != nil {
			fmt.Println("instructions not deleted")
			return fmt.Errorf("server: sqlite_db_insert: failed to delete instructions from db: %w", err)
		}
		fmt.Println("instructions deleted")

		return nil
	}); err != nil {
		return fmt.Errorf("server: sqlite_db_insert: insertTestData transaction failed: %w", err)
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
