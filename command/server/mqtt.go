package server

import (
	"fmt"

	"go.uber.org/zap"
)

type MQTT interface {
	publish(topic string, data string, qos byte)
	publishDriveInstructionSequence(instructionSequence driveInstructions)
}

func (m *MQTTClient) publishDriveInstructionSequence(instructionSequence driveInstructions) {
	driveInstructionDelimiter := ":"
	topic := "/drive/instruction"
	var qos byte = 2 // Guarantee delivery

	for _, instruction := range instructionSequence {
		encodedInstruction := fmt.Sprintf("%s%s%d", instruction.instruction, driveInstructionDelimiter, instruction.value)
		m.publish(topic, encodedInstruction, qos)
	}

	m.publish(topic, "X", qos)

	m.logger.Info("published drive instruction sequence successfully", zap.Array("instructionSequence", &instructionSequence))
}
