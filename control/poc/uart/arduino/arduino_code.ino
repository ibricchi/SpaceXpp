enum Key {
    NUMBER = 0,
    MSG = 1,
};

void setup() {
  Serial.begin(9600);
}

void loop() {
    Serial.println("Hello from Arduino");

    if (Serial.available()) {
      char key = Serial.read();
      String data_rcvd;
      switch(key) {
        case NUMBER:
          // Expecting single byte
          data_rcvd = Serial.read();
          Serial.print("NUMBER: ");
          Serial.println(data_rcvd);
          break;
        case MSG:
          data_rcvd = Serial.readString();
          Serial.print("MSG: ");
          Serial.println(data_rcvd);
          break;
        default:
          Serial.println("INVALID KEY!!!");
          data_rcvd = Serial.readString();
          Serial.print("INVALID DATA: ");
          Serial.println(data_rcvd);
          break;
      }
    }

    delay(100);
}
