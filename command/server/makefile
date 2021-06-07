all: credentials server

credentials:
	go build cmd/credentials/main.go
	mv main bin/credentials

server:
	go build cmd/server/main.go
	mv main bin/server

.PHONY: clean

clean: 
	rm -f bin/*
