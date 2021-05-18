package main

import (
	"context"
	"flag"
	"fmt"
	"log"

	"github.com/IBricchi/SpaceXpp/blob/bsc/httpserver1/command/server"
	"github.com/go-chi/chi"
	"go.uber.org/zap"
)

func main() {

	fmt.Println("Loading Server")

	var httpPort = flag.String("httpPort", "3000", "Port for serving http server")
	var serverDBFilePath = flag.String("db", "serverDB.db", "SQLite DB file path")
	flag.Parse()

	serverDBDSN := "db/" + *serverDBFilePath

	ctx := context.Background()

	logger, err := zap.NewDevelopment()
	if err != nil {
		log.Fatalf("server: failed to create zap logger: %v\n", err)
	}
	defer logger.Sync()

	fmt.Println("Server successfully created a zap logger")

	/* Database stuff
	serverDB, err := server.OpenSQLiteDB(ctx, logger, serverDBDSN)
	if err != nil {
		logger.Fatal("server: failed to open SQLite server database", zap.Error(err))
	}
	logger.Info("server: opened server sqlite3 DB")
	*/

	r := chi.NewRouter()

	httpServer := server.OpenHttpServer(ctx, logger, r, serverDB)
	defer httpServer.Close()

	logger.Info("server: opened http server")
	fmt.Fprint("Server successfully opened http server")

	fmt.Fprint("going into loop of listening")
	if err := httpServer.Serve(ctx, *httpPort); err != nil {
		logger.Fatal("server: failed to serve http server", zap.Error(err))
	}

}
