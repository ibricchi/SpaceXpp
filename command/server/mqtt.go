package server

type MQT interface {
	filler() error
}

func (m *MQTTClient) filler() error {
	return nil
}
