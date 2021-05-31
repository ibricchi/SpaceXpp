const byte numUARTChars = 40;
char receivedUARTChars[numUARTChars];
boolean newUARTDataAvailable = false;

unsigned long previousMillis = 0;

void setup() {
    Serial.begin(115200);

    Serial.print("Arduino Setup Complete\n");
}

void loop() {
    recvUARTWithStartEndMarkers();
    processNewUARTData();

    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis > 5000) {
      previousMillis = currentMillis;
      Serial.print("R"); // Ready for next instruction signal => should be send after finished with current drive instruction
    }
}

void recvUARTWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte idx = 0;
    const char startMarker = '<';
    const char endMarker = '>';
    char received;
 
    while (Serial.available() && !newUARTDataAvailable) {
        received = Serial.read();

        if (recvInProgress) {
            if (received != endMarker) {
                receivedUARTChars[idx++] = received;
                
                if (idx >= numUARTChars) {
                    idx = numUARTChars - 1;
                    Serial.print("ERROR: UART character buffer not large enough\n");
                }
            } else {
                receivedUARTChars[idx] = '\0'; // terminate the string
                recvInProgress = false;
                idx = 0;
                newUARTDataAvailable = true;
            }
        } else if (received == startMarker) {
            recvInProgress = true;
        }
    }
}

void processNewUARTData() {
    if (newUARTDataAvailable) {
        // Read and pop instruction key
        const char instructionKey = receivedUARTChars[strlen(receivedUARTChars)-1];
        receivedUARTChars[strlen(receivedUARTChars)-1] = '\0';

        switch(instructionKey) {
          case 'F':
            Serial.print("FORWARD VALUE: ");
            Serial.print(receivedUARTChars);
            break;
          case 'R':
            Serial.print("TURN RIGHT VALUE: ");
            Serial.print(receivedUARTChars);
            break;
          case 'L':
            Serial.print("TURN LEFT VALUE: ");
            Serial.print(receivedUARTChars);
            break;
          default:
            Serial.print("INVALID INSTRUCTION KEY\n");
            break;
        }

        newUARTDataAvailable = false;
    }
}
