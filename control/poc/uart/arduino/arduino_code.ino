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
    if (currentMillis - previousMillis > 1000) {
      previousMillis = currentMillis;
      Serial.println("<Data from Arduino>");
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
                receivedUARTChars[idx] = received;
                idx++;
                
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
        // Read and pop key
        const char key = receivedUARTChars[strlen(receivedUARTChars)-1];
        receivedUARTChars[strlen(receivedUARTChars)-1] = '\0';

        // Data encoding defined in esp32 => Will probably want both way data encoding
        switch(key) {
          case 'F':
            Serial.print("FORWARD VALUE: ");
            Serial.print(receivedUARTChars);
            break;
          case 'T':
            Serial.print("TURN VALUE: ");
            Serial.print(receivedUARTChars);
            break;
          default:
            Serial.print("INVALID KEY\n");
            break;
        }
        
        newUARTDataAvailable = false;
    }
}
