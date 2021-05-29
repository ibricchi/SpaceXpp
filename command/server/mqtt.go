package server

import (
	"fmt"
)

type MQTT interface {
	publish(topic string, data string, qos byte)
	publishDriveInstructionSequence(instructionSequence []driveInstruction)
}

func (m *MQTTClient) publishDriveInstructionSequence(instructionSequence []driveInstruction) {
	driveInstructionDelimiter := ":"
	topic := "/drive/instruction"
	var qos byte = 2 // Guarantee delivery

	for _, instruction := range instructionSequence {
		encodedInstruction := fmt.Sprintf("%s%s%d", instruction.instruction, driveInstructionDelimiter, instruction.value)
		m.publish(topic, encodedInstruction, qos)
	}
}
