package server

import "fmt"

type DB interface {
	createTable() error
}

func (s *SQLiteDB) createTable() error {
	fmt.Println("Stating db tests")
	statement, _ := s.db.Prepare("CREATE TABLE IF NOT EXISTS people (id INTEGER PRIMARY KEY, name TEXT)")
	statement.Exec()

	return nil
}
