package server

import (
	"context"
	"crypto/tls"
	"crypto/x509"
	"fmt"
	"io/ioutil"
	"log"

	mqtt "github.com/eclipse/paho.mqtt.golang"
	"go.uber.org/zap"
)

type MQTTClient struct {
	client mqtt.Client
	logger *zap.Logger
}

func InitMQTT(ctx context.Context, logger *zap.Logger, mqttBrokerURL string) (*MQTTClient, error) {
	tlsConfig, err := NewTlsConfig()
	if err != nil {
		return &MQTTClient{}, fmt.Errorf("server: mqtt: failed to get TLS config: %w", err)
	}

	opts := mqtt.NewClientOptions()
	opts.AddBroker(mqttBrokerURL)
	opts.SetTLSConfig(tlsConfig)
	opts.SetClientID("SpaceXpp_server")

	// opts.SetUsername(*mqttUsername)
	// opts.SetPassword(*mqttPassword)

	opts.OnConnect = mqttConnectHandler
	opts.OnConnectionLost = mqttConnectLostHandler

	return &MQTTClient{
		client: mqtt.NewClient(opts),
		logger: logger,
	}, nil
}

func (m *MQTTClient) Connect() error {
	if token := m.client.Connect(); token.Wait() && token.Error() != nil {
		return fmt.Errorf("server: mqtt: failed to connect to broker: %w", token.Error())
	}
	return nil
}

var testStatusMessagePubHandler mqtt.MessageHandler = func(client mqtt.Client, msg mqtt.Message) {
	fmt.Printf("Received message: %s from topic: %s\n", msg.Payload(), msg.Topic())
}

var mqttConnectHandler mqtt.OnConnectHandler = func(client mqtt.Client) {
	fmt.Println("Connected to MQTT broker successfully")

	// Subscribe to topics
	if token := client.Subscribe("/test/status", 0, testStatusMessagePubHandler); token.Wait() && token.Error() != nil {
		log.Fatalf("server: mqtt: failed to subscribe to /test/status: %v", token.Error())
	}
	fmt.Println("Subscribed to topic: /test/status")
}

var mqttConnectLostHandler mqtt.ConnectionLostHandler = func(client mqtt.Client, err error) {
	fmt.Printf("Connect to MQTT broker lost: %v", err)
}

func NewTlsConfig() (*tls.Config, error) {
	certpool := x509.NewCertPool()

	ca, err := ioutil.ReadFile("cert/mqtt_ca_cert.pem")
	if err != nil {
		return nil, fmt.Errorf("server: mqtt: failed to read CA certificate: %w", err)
	}

	certpool.AppendCertsFromPEM(ca)

	return &tls.Config{
		RootCAs: certpool,
	}, nil
}

func (m *MQTTClient) publish(topic string, data string, qos byte) {
	token := m.client.Publish(topic, qos, false, data)
	token.Wait()
}
