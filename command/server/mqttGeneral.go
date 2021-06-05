package server

import (
	"context"
	"crypto/tls"
	"crypto/x509"
	"fmt"
	"io/ioutil"
	"log"
	"strconv"
	"strings"

	mqtt "github.com/eclipse/paho.mqtt.golang"
	"go.uber.org/zap"
)

type MQTTClient struct {
	client mqtt.Client
	logger *zap.Logger
}

func InitMQTT(ctx context.Context, logger *zap.Logger, db *SQLiteDB, mqttBrokerURL string, mqttUsername string, mqttPassword string) (*MQTTClient, error) {
	tlsConfig, err := NewTlsConfig()
	if err != nil {
		return &MQTTClient{}, fmt.Errorf("server: mqtt: failed to get TLS config: %w", err)
	}

	opts := mqtt.NewClientOptions()
	opts.AddBroker(mqttBrokerURL)
	opts.SetTLSConfig(tlsConfig)
	opts.SetUsername(mqttUsername)
	opts.SetPassword(mqttPassword)
	opts.SetClientID("SpaceXpp_server")
	opts.SetOrderMatters(true) // Drive instruction order must be preserved
	opts.SetCleanSession(true)
	opts.SetConnectRetry(true)

	opts.OnConnect = mqttConnectHandler(logger, ctx, db)
	opts.OnConnectionLost = mqttConnectLostHandler

	return &MQTTClient{
		client: mqtt.NewClient(opts),
		logger: logger,
	}, nil
}

func (m *MQTTClient) getLogger() *zap.Logger {
	return m.logger
}

func (m *MQTTClient) Connect() error {
	if token := m.client.Connect(); token.Wait() && token.Error() != nil {
		return fmt.Errorf("server: mqtt: failed to connect to broker: %w", token.Error())
	}
	return nil
}

func (m *MQTTClient) Disconnect() {
	m.client.Disconnect(100)

	m.logger.Info("Disconnected from MQTT broker successfully")
}

var testStatusMessagePubHandler mqtt.MessageHandler = func(client mqtt.Client, msg mqtt.Message) {
	fmt.Printf("Received message: %s from topic: %s\n", msg.Payload(), msg.Topic())
}

func mqttConnectHandler(logger *zap.Logger, ctx context.Context, *SQLiteDB) mqtt.OnConnectHandler {
	return func(client mqtt.Client) {
		fmt.Println("Connected to MQTT broker successfully")

		// Subscribe to topics
		if token := client.Subscribe("/test/status", 0, testStatusMessagePubHandler); token.Wait() && token.Error() != nil {
			log.Fatalf("server: mqtt: failed to subscribe to /test/status: %v", token.Error())
		}
		fmt.Println("Subscribed to topic: /test/status")

		// Subscribe to topics
		if token := client.Subscribe("/feedback/instruction", 2, instructionFeedPubHandler(logger, ctx, db)); token.Wait() && token.Error() != nil {
			log.Fatalf("server: mqtt: failed to subscribe to /feedback/instruction: %v", token.Error())
		}
		fmt.Println("Subscribed to topic: /feedback/instruction")
	}
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
	go func() {
		token.Wait()
		if err := token.Error(); err != nil {
			m.logger.Error("server: mqttGeneral: failed to publish mqtt message", zap.Error(err))
		}
	}()
}

func (m *MQTTClient) publishDriveInstructionSequence(instructionSequence driveInstructions) {
	driveInstructionDelimiter := ":"
	topic := "/drive/instruction"
	var qos byte = 2 // Guarantee delivery

	for _, instruction := range instructionSequence {
		encodedInstruction := fmt.Sprintf("%s%s%d", instruction.Instruction, driveInstructionDelimiter, instruction.Value)
		m.publish(topic, encodedInstruction, qos)
	}

	m.publish(topic, "X", qos)

	m.logger.Info("published drive instruction sequence successfully", zap.Array("instructionSequence", &instructionSequence))
}

// Subscribing to instruction feed
var stopData string

func instructionFeedPubHandler(logger *zap.Logger, ctx context.Context, db *SQLiteDB) mqtt.MessageHandler {
	return func(client mqtt.Client, msg mqtt.Message) {
		fmt.Printf("Received message: %s from topic: %s\n", msg.Payload(), msg.Topic())

		s := strings.Split(string(msg.Payload()), ":")
		value := s[1]
		v, _ := strconv.Atoi(value) // No error checking as this is supposed to fail for stop instructions

		var instruction driveInstruction
		if s[0] == "F" {
			instruction.Instruction = "forward"
			instruction.Value = v
			updateMap(instruction, ctx, db)
		} else if s[0] == "R" {
			instruction.Instruction = "turnRight"
			instruction.Value = v
			updateMapWithObstructionWhileTurning("")
			updateMap(instruction, ctx, db)
		} else if s[0] == "L" {
			instruction.Instruction = "turnLeft"
			instruction.Value = v
			updateMapWithObstructionWhileTurning("")
			updateMap(instruction, ctx, db)
		} else if s[0] == "X" {
			instruction.Instruction = "nil"
			instruction.Value = 0
			updateMap(instruction, ctx, db)
		} else if s[0] == "S" {
			if stashedDriveInstruction.Instruction == "forward" { // wait for second part of stop instruction to update map and stop
				stopData = value
			} else { // turning => update map without stopping
				updateMapWithObstructionWhileTurning(value)
			}
		} else if s[0] == "SD" {
			// Need to create MQTTClient for calling methods on it
			mqttClient := &MQTTClient{
				client: client,
				logger: logger,
			}

			if v == -1 { // stopping after turn (map already updated with obstruction)
				stop(mqttClient, ctx, db, 0, stopData, true)
			} else { // stopping after forward (map not yet updated with obstruction)
				stop(mqttClient, ctx, db, v, stopData, false)
			}

			stopData = ""
		} else if s[0] == "B" {
			// Ignore backwards instruction that are used for distance correction (drive only)
		} else {
			fmt.Println("server: mqttGeneral: unknown drive instruction")
		}
	}
}
