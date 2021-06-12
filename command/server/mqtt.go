/*
   Written by Bradley Stanley-Clamp (bradley.stanley-clamp19@imperial.ac.uk), 2021 - SpaceX++ EEE/EIE 2nd year group project, Imperial College London
*/

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
