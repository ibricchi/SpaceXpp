package server

type MQ interface {
	filler() error
}

func (m *MQTTClient) filler() error {
	return nil
}
