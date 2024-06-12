const int pirPin = 2;    // Pin connected to the PIR motion sensor
void setup() {
  Serial.begin(9600);    // Initialize serial communication at 9600 baud
  pinMode(pirPin, INPUT); // Set the PIR sensor pin as input
}

void loop() {
  int motionDetected = digitalRead(pirPin);  // Read the PIR sensor
  
  // Print 1 if motion is detected, otherwise print 0
  if (motionDetected == HIGH) {
    Serial.println(1);
  } else {
    Serial.println(0);
  }

  delay(500);
}
