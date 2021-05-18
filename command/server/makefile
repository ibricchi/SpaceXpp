all: server

server:
	go build cmd/server/main.go
	mv main bin/server

.PHONY: clean

clean: 
	rm -f bin/*
