package main

import (
	"context"
	"flag"
	"log"

	"github.com/IBricchi/SpaceXpp/command/server"
	"go.uber.org/zap"
)

func main() {
	var mqttBrokerURL = flag.String("mqttBrokerURL", "ssl://18.130.239.157:8883", "URL of MQTT Broker")
	flag.Parse()

	ctx := context.Background()

	logger, err := zap.NewDevelopment()
	if err != nil {
		log.Fatalf("server: failed to create zap logger: %v\n", err)
	}
	defer logger.Sync()

	mqttClient, err := server.InitMQTT(ctx, logger, *mqttBrokerURL)
	if err != nil {
		logger.Fatal("server: failed to init MQTT client", zap.Error(err))
	}

	if err := mqttClient.Connect(); err != nil {
		logger.Fatal("server: MQTT client failed to connect to broker", zap.Error(err))
	}
	defer mqttClient.Disconnect()

	// Make mqttClient a member of the main server struct
	for true {
	}
}
