package server

import "go.uber.org/zap"

type MQTT interface {
	getLogger() *zap.Logger
	Connect() error
	Disconnect()
	publish(topic string, data string, qos byte)
	publishDriveInstructionSequence(instructionSequence driveInstructions)
	getIsConnected() bool
}
