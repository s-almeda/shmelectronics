/* 

Creates a wifi network and website served by the ESP32
The website is an interface that takes "binary" code commands
Each command is added to a looping program 
To turns on/off various devices!

Created by Shm! (+ chatgpt)

HARDWARE
as written, currently it expects a NodeMCU ESP32 connected to...
- 3 LEDs (with a ~220ohm resistor for each) 
- a small continuous rotation servo FS90R 
- a larger 180degree servo */

// Include the necessary libraries
#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>
#include <vector>

// Create an instance of the WebServer on port 80
WebServer server(80);

// Define GPIO pins


const int ledRedPin = 17;  //on my breadboard, 11

const int ledGreenPin = 16;  //breadboard 12
const int ledBluePin = 4;    //breadboard 13

const int bigServoPin = 26;    //breadboard 10
const int smallServoPin = 12;  //breadboard 13, left

// Variable states
bool redLedState = false;
bool greenLedState = false;
bool blueLedState = false;
bool bigServoState = false;
bool smallServoState = false;
bool isPaused = false;

std::vector<String> commandList;
std::vector<String> commandNames;
size_t currentCommandIndex = 0;

// Servo objects
Servo bigServo;
Servo smallServo;

void executeCommand(String command) {
  if (command.length() == 5) {
    redLedState = command[0] == '1';
    greenLedState = command[1] == '1';
    blueLedState = command[2] == '1';
    smallServoState = command[3] == '1';
    bigServoState = command[4] == '1';

    digitalWrite(redLED, HIGH);

    digitalWrite(ledRedPin, redLedState ? HIGH : LOW);
    digitalWrite(ledGreenPin, greenLedState ? HIGH : LOW);
    digitalWrite(ledBluePin, blueLedState ? HIGH : LOW);

    bigServo.write(bigServoState ? 170 : 10);
    if (smallServoState) {
      analogWrite(smallServoPin, 45); // Spin 
    } else {
      analogWrite(smallServoPin, 0); // Stop the servo
    }
    //smallServo.write(smallServoState ? 60 : 90);  // 0 for spin, 90 for stop
  }
}

void handleRoot() {
  String html = "<!DOCTYPE html>\n<html>\n<head>\n<title>Control Shm's Electronics!</title>\n<style>\nbody { font-family: sans-serif; }\ntextarea { font-size: 1.5em; width: 50%; height: 25px; margin-bottom: 10px; }\nbutton { font-size: 1.2em; margin-top: 10px; }\n.message { color: green; font-weight: bold; }\n.error { color: red; font-weight: bold; }\n</style>\n</head>\n<body>\n<h1>Let's program my devices!</h1>\n";
  html += "<h4>Go to your wifi settings and join the network called <em>shm_zone</em> (the password is <em>password</em></h4> <h4>Then, visit the website <em>192.168.4.1</em> with your internet browser.</h4>";
  html += "<p>Each digit will turn a device on or off,</p><p>depending on whether it is a <strong>1</strong> or a <strong>0</strong>.</p><p>The \"switches\" are in this order:</p>\n<p>[red] [green] [blue] [small motor] [big motor]</p>\n";

  html += "<textarea id='binaryInput' placeholder='Enter up to 5 digits (0 or 1)'></textarea><br>\n";
  html += "<textarea id='nameInput' placeholder='Enter your name'></textarea><br>\n";
  html += "<button onclick=\"submitCommand()\">Submit Command</button>\n";
  html += "<p id='feedbackMessage'></p>\n";

  html += "<p>(Reload to see other updates to the program!)</p>\n";

  html += "<h2>Current program:</h2>\n<ol>\n";
  for (size_t i = 0; i < commandList.size(); i++) {
    html += "<li>Command " + String(i + 1) + ": " + commandList[i] + " - " + commandNames[i];
    html += " <button onclick=\"deleteCommand(" + String(i) + ")\">Delete</button></li>\n";
  }
  html += "<li>Command " + String(commandList.size() + 1) + "...repeat the entire program from the top!</li>";
  html += "</ol>\n";

  html += "<script>\n";
  html += "function submitCommand() {\n";
  html += "  let binaryInput = document.getElementById('binaryInput').value;\n";
  html += "  const nameInput = document.getElementById('nameInput').value;\n";
  html += "  const feedback = document.getElementById('feedbackMessage');\n";
  html += "  if (/[^01]/.test(binaryInput)) {\n";
  html += "    feedback.textContent = 'Oops! We can only use 1s and 0s to switch the devices on (1) and off (0)!';\n";
  html += "    feedback.className = 'error';\n";
  html += "    return;\n";
  html += "  }\n";
  html += "  if (binaryInput.length > 5) {\n";
  html += "    binaryInput = binaryInput.substring(0, 5);\n";
  html += "  } else {\n";
  html += "    while (binaryInput.length < 5) {\n";
  html += "      binaryInput += '0';\n";
  html += "    }\n";
  html += "  }\n";
  html += "  if (!nameInput.trim()) {\n";
  html += "    feedback.textContent = 'Hey there! Please add your name so we know who is submitting.';\n";
  html += "    feedback.className = 'error';\n";
  html += "    return;\n";
  html += "  }\n";
  html += "  fetch('/binaryCommand?cmd=' + binaryInput + '&name=' + encodeURIComponent(nameInput))\n";
  html += "    .then(() => {\n";
  html += "      feedback.textContent = 'Command added! Great job! Reloading to see updates...';\n";
  html += "      feedback.className = 'message';\n";
  html += "      setTimeout(() => location.reload(), 1000);\n";
  html += "    })\n";
  html += "    .catch(() => {\n";
  html += "      feedback.textContent = 'Oops! Something went wrong. Try again!';\n";
  html += "      feedback.className = 'error';\n";
  html += "    });\n";
  html += "}\n";

  html += "function deleteCommand(index) { fetch('/deleteCommand?index=' + index).then(() => { location.reload(); }); }\n";
  html += "</script>\n";

  html += "<div id='adminControls' style='display:none;'>\n";
  html += "<button onclick=\"clearInputs()\">Clear Inputs</button>\n";
  html += "<button onclick=\"togglePause()\">Pause/Resume</button>\n";
  html += "</div>\n";

  html += "</body>\n</html>";

  server.send(200, "text/html", html);
}


void handleBinaryCommand() {
  if (server.hasArg("cmd") && server.hasArg("name")) {
    String command = server.arg("cmd");
    String name = server.arg("name");
    while (command.length() < 5) {
      command += "0";
    }
    if (command.length() == 5) {
      if (commandList.size() >= 10) {
        commandList.erase(commandList.begin());  // Remove the oldest command
        commandNames.erase(commandNames.begin());
      }
      commandList.push_back(command);
      commandNames.push_back(name);
      server.send(200, "text/plain", "Command added successfully!");
    } else {
      server.send(400, "text/plain", "Hmm, something went wrong. Double-check the command and try again.");
    }
  } else {
    server.send(400, "text/plain", "Looks like you missed entering the command or name. Please try again.");
  }
}

void handleClearInputs() {
  commandList.clear();
  commandNames.clear();
  currentCommandIndex = 0;
  digitalWrite(ledRedPin, LOW);
  digitalWrite(ledGreenPin, LOW);
  digitalWrite(ledBluePin, LOW);
  bigServo.write(10);    // Reset to off position
  //smallServo.write(90);  // Reset to stop
  analogWrite(smallServoPin, 0);
  server.send(200, "text/plain", "All inputs cleared, and devices have been reset. Ready for new commands!");
}
void handleDeleteCommand() {
  if (server.hasArg("index")) {
    int index = server.arg("index").toInt();
    if (index >= 0 && index < commandList.size()) {
      commandList.erase(commandList.begin() + index);
      commandNames.erase(commandNames.begin() + index);
      server.send(200, "text/plain", "Command successfully deleted.");
    } else {
      server.send(400, "text/plain", "That command does not seem to exist. Please try again.");
    }
  } else {
    server.send(400, "text/plain", "Oops! You forgot to specify which command to delete.");
  }
}

void handleTogglePause() {
  isPaused = !isPaused;
  server.send(200, "text/plain", isPaused ? "Paused" : "Resumed");
}

void executeCommands() {
  if (!isPaused && !commandList.empty()) {
    executeCommand(commandList[currentCommandIndex]);
    currentCommandIndex = (currentCommandIndex + 1) % commandList.size();
    delay(600);  // Wait for 0.6 seconds...
    if (smallServoState) { //check if the small servo is on
      analogWrite(smallServoPin, 0); // turn that guy back off  
    }
    delay(600); //delay another 0.6 seconds
  }
}

void setup() {
  // Initialize GPIO pins
  pinMode(ledRedPin, OUTPUT);
  pinMode(ledGreenPin, OUTPUT);
  pinMode(ledBluePin, OUTPUT);

  digitalWrite(ledRedPin, LOW);
  digitalWrite(ledGreenPin, LOW);
  digitalWrite(ledBluePin, LOW);

  // Attach servos
  smallServo.attach(smallServoPin);
  //smallServo.write(90);  // Start stopped
  analogWrite(smallServoPin, 0);
  bigServo.attach(bigServoPin);
  bigServo.write(0);

  // Start serial communication
  Serial.begin(115200);

  // Start Wi-Fi in Access Point mode
  WiFi.softAP("shm_zone", "password");
  IPAddress IP = WiFi.softAPIP();
  Serial.print("Access Point IP: ");
  Serial.println(IP);

  // Define URL handlers
  server.on("/", handleRoot);
  server.on("/binaryCommand", handleBinaryCommand);
  server.on("/clearInputs", handleClearInputs);
  server.on("/togglePause", handleTogglePause);
  server.on("/deleteCommand", handleDeleteCommand);

  // Start the server
  server.begin();
  Serial.println("HTTP server started");
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
}

void loop() {
  // Handle client requests
  server.handleClient();
  executeCommands();
}
