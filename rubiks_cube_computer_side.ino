// Variable Declarations
String myname, outString;
const String samp = "L3 U1 B1 R2 F3 L1 F3 U2 L1 U3 B3 U2 B1 L2 F1 U2 R2 L2 B2 (19f)\n";
const int led = 13;
int i;

void setup() {
  // Serial communication setup
  Serial.begin(9600);
  Serial.println("");
  
  // Set LED pin as output
  pinMode(led, OUTPUT);
}

void loop() {
  // Turn off the LED initially
  digitalWrite(led, LOW);

  // Read characters from Serial
  while (Serial.available() > 0) {
    char rec = Serial.read();
    myname += rec;

    // If a newline character is received
    if (rec == '\n') {
      // Check if the received string matches the sample string
      if (myname.equals(samp)) {
        digitalWrite(led, HIGH); // Turn on the LED
        delay(5000); // Wait for 5 seconds
        digitalWrite(led, LOW); // Turn off the LED
      }

      myname.remove(myname.length() - 6, 6); // Remove last 6 characters from the string

      // Process the string in groups of 2 characters
      for (i = 0; i < myname.length(); i += 3) {
        outString = String(myname[i]) + '\t' + String(myname[i + 1]);
      }

      myname = ""; // Reset the string
    }
  }
}
