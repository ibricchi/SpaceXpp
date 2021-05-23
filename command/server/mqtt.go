package server

type MQTT interface {
	publish(topic string, data string, qos byte)
}
