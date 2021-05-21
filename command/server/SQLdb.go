package server

import (
	"context"
	"fmt"
)

type DB interface {
	createTable(ctx context.Context) error
	insertData(status bool, battery int) error
	retriveData() (error, bool, int)
	Close() error
}

func (s *SQLiteDB) createTable(ctx context.Context) error {
	fmt.Println("Creating summary table")
	statement, _ := s.db.Prepare("CREATE TABLE IF NOT EXISTS summary (id INTEGER PRIMARY KEY, battery INTEGER)")
	statement.Exec()

	return nil
}

func (s *SQLiteDB) insertData(status bool, battery int) error {
	//statement, _ := s.db.Prepare("CREATE TABLE IF NOT EXISTS summary (id INTEGER PRIMARY KEY, battery INTEGER)")
	//statement.Exec()
	statement, _ := s.db.Prepare("INSERT INTO summary (battery) VALUES (?)")
	statement.Exec(battery)
	defer statement.Close()

	return nil
}

func (s *SQLiteDB) retriveData() (error, bool, int) {
	rows, _ := s.db.Query("SELECT MAX(id), battery FROM summary")
	var id int
	var batVal int
	rows.Next()
	rows.Scan(&id, &batVal)

	return nil, true, batVal

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
