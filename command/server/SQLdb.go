package server

import (
	"context"
	"fmt"
	"strconv"
)

type DB interface {
	createTable(ctx context.Context) error
	insertData(status bool, battery int) error
	retriveData() (error, bool, int)
	Close() error
}

func (s *SQLiteDB) createTable(ctx context.Context) error {
	fmt.Println("Creating summary table")
	statement, _ := s.db.Prepare("CREATE TABLE IF NOT EXISTS summary (id INTEGER PRIMARY KEY, name TEXT)")
	statement.Exec()

	return nil
}

func (s *SQLiteDB) insertData(status bool, battery int) error {

	statement, _ := s.db.Prepare("INSERT INTO summary (name) VALUES (?)")
	statement.Exec("Brad")

	fmt.Println("data in db, now querying")

	rows, _ := s.db.Query("SELECT id, name FROM people")
	var id int
	var name string
	for rows.Next() {
		rows.Scan(&id, &name)
		fmt.Println(strconv.Itoa(id) + ": " + name)
	}

	/*batVal := strconv.Itoa(battery)
	batValt := batVal + batVal

	s.db.Prepare("INSERT INTO summary (battery) VALUES (" + batVal + ")")
	s.db.Prepare("INSERT INTO summary (battery) VALUES (" + batValt + ")")

	fmt.Println("Indserted: ")

	rows, _ := s.db.Query("SELECT id, battery FROM summary")
	var id int
	var val int
	for rows.Next() {
		rows.Scan(&id, &val)
		fmt.Println(strconv.Itoa(id) + ": " + strconv.Itoa(val))
	}
	*/
	return nil
}

func (s *SQLiteDB) retriveData() (error, bool, int) {

	latest, err := s.db.Query("SELECT id, battery FROM summary")
	if err != nil {
		return err, false, -1
	}
	var batVal int
	var id int
	for latest.Next() {
		latest.Scan(&id, &batVal)
		fmt.Println("retrived: ")
		fmt.Println(batVal)
	}

	fmt.Println("retrived: ")
	fmt.Println(batVal)

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
