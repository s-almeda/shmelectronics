/* Wi-fi connected web interface!
Makes your NodeMCU ESP32...
 - Connect to a wifi network (i.e. your iphone's hotspot)
 - Serve a web interface, and print the URL to access it via serial 
 (open your Serial Monitor, and paste the URL into any internet browser!)
 - The web interface has buttons that should turn on/off connected devices!
created with love by shm :3
*/
// Include the necessary libraries
#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

// Replace with your iPhone hotspot credentials
const char* ssid = "hakkero";
const char* password = "wifipassword";

// Create an instance of the WebServer on port 80
WebServer server(80);

// Define GPIO pins
const int ledRedPin = 12;
const int ledGreenPin = 4;
const int bigServoPin = 26;
const int smallServoPin = 5;

// Variable states
bool redLedState = false;
bool greenLedState = false;
bool bigServoState = false;
bool smallServoState = false;

// Servo objects
Servo bigServo;
Servo smallServo;

// Function to handle the root page
void handleRoot() {
  String html = "<!DOCTYPE html>\n<html>\n<head>\n<title>ESP32 Control</title>\n</head>\n<body>\n<h1>ESP32 Control</h1>\n";

  html += "<h2>LED Controls</h2>\n";
  html += "<p>Red LED is currently: <strong>";
  html += (redLedState ? "ON" : "OFF");
  html += "</strong></p>\n";
  html += "<button onclick=\"toggleRedLED()\">";
  html += (redLedState ? "Turn OFF" : "Turn ON");
  html += "</button><br><br>\n";

  html += "<p>Green LED is currently: <strong>";
  html += (greenLedState ? "ON" : "OFF");
  html += "</strong></p>\n";
  html += "<button onclick=\"toggleGreenLED()\">";
  html += (greenLedState ? "Turn OFF" : "Turn ON");
  html += "</button><br><br>\n";

  html += "<h2>Servo Controls</h2>\n";
  html += "<p>Big Servo is currently: <strong>";
  html += (bigServoState ? "Swept to 180°" : "At 0°");
  html += "</strong></p>\n";
  html += "<button onclick=\"toggleBigServo()\">Sweep Big Servo</button><br><br>\n";

  html += "<p>Small Servo is currently: <strong>";
  html += (smallServoState ? "ON" : "OFF");
  html += "</strong></p>\n";
  html += "<button onclick=\"toggleSmallServo()\">";
  html += (smallServoState ? "Turn OFF" : "Turn ON");
  html += "</button><br><br>\n";

  html += "<script>\n";
  html += "function toggleRedLED() { fetch('/toggleRed').then(() => { location.reload(); }); }\n";
  html += "function toggleGreenLED() { fetch('/toggleGreen').then(() => { location.reload(); }); }\n";
  html += "function toggleBigServo() { fetch('/toggleBigServo').then(() => { location.reload(); }); }\n";
  html += "function toggleSmallServo() { fetch('/toggleSmallServo').then(() => { location.reload(); }); }\n";
  html += "</script>\n";

  html += "</body>\n</html>";

  server.send(200, "text/html", html);
}

// Functions to toggle LEDs
void handleToggleRedLED() {
  redLedState = !redLedState;
  digitalWrite(ledRedPin, redLedState ? HIGH : LOW);
  server.send(200, "text/plain", "OK");
}

void handleToggleGreenLED() {
  greenLedState = !greenLedState;
  digitalWrite(ledGreenPin, greenLedState ? HIGH : LOW);
  server.send(200, "text/plain", "OK");
}

// Functions to control servos
void handleToggleBigServo() {
  bigServoState = !bigServoState;
  bigServo.write(bigServoState ? 180 : 0);
  delay(1000); // Wait for the servo to complete the sweep
  server.send(200, "text/plain", "OK");
}

void handleToggleSmallServo() {
  smallServoState = !smallServoState;
  smallServo.write(smallServoState ? 0 : 90); // 0 for spin, 90 for stop
  server.send(200, "text/plain", "OK");
}

void setup() {
  // Initialize GPIO pins
  pinMode(ledRedPin, OUTPUT);
  pinMode(ledGreenPin, OUTPUT);

  digitalWrite(ledRedPin, LOW);
  digitalWrite(ledGreenPin, LOW);


  // Attach big servo
  smallServo.attach(smallServoPin);
  smallServo.write(90); // Start stopped
  bigServo.attach(bigServoPin);
  bigServo.write(0);

  // Start serial communication
  Serial.begin(115200);

  // Connect to Wi-Fi
  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");
  Serial.println(WiFi.localIP());

  // Define URL handlers
  server.on("/", handleRoot);
  server.on("/toggleRed", handleToggleRedLED);
  server.on("/toggleGreen", handleToggleGreenLED);
  server.on("/toggleBigServo", handleToggleBigServo);
  server.on("/toggleSmallServo", handleToggleSmallServo);

  // Start the server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  // Handle client requests
  server.handleClient();
}
