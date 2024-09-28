

# ESP8266 Water Tank Motor Control

This project controls a water tank motor via an ESP8266 using a web interface. It provides a user-friendly UI for controlling the motor based on predefined durations, and allows manual stopping of the motor. It also includes captive portal functionality to make it easier for users to connect to the device's Wi-Fi network.

## Code Breakdown

### Libraries
```cpp
#include <ESP8266WiFi.h>       // Wi-Fi functionality
#include <ESP8266WebServer.h>  // HTTP server functionality
#include <ESP8266mDNS.h>       // mDNS functionality for local hostname
#include <DNSServer.h>         // DNS server for captive portal

- ESP8266WiFi.h: Manages the Wi-Fi connection.
- ESP8266WebServer.h: Handles HTTP requests and serves the web page.
- ESP8266mDNS.h: Allows the device to be accessed using a local hostname (`hostname.local`).
- DNSServer.h: Redirects all DNS queries to the ESP8266 to serve a captive portal, useful for devices like smartphones.

### Pin Definitions and Web Server Setupconst int motorPin = 5;  // Motor control pin
ESP8266WebServer server(80);  // Web server on port 80
DNSServer dnsServer;          // DNS server for captive portal
const byte DNS_PORT = 53;     // DNS port number

- motorPin: GPIO pin connected to the relay or motor driver.
- server(80): Creates an HTTP server on port 80.
- dnsServer: DNS server used for captive portal.
- DNS_PORT: Specifies the port for DNS (53 by default).

### Wi-Fi and mDNS Configurationconst char* ssid = "ESP8266_WaterControl";  // Wi-Fi SSID
const char* password = "12345678";          // Wi-Fi password
const char* hostname = "ErShubham";         // mDNS hostname (e.g., `shubham.local`)

- ssid: Name of the Wi-Fi network that the ESP8266 will create.
- password: Password to join the network.
- hostname: Local hostname for accessing the device (`hostname.local`).

### HTML Web Interfaceconst char index_html[] PROGMEM = R"rawliteral(
  <html> <!-- Full HTML omitted for brevity -->
</html>
)rawliteral";

The HTML interface allows the user to:
- Control the motor for different durations (1, 5, 10, 30 minutes, etc.).
- Stop the motor immediately using a "STOP MOTOR" button.
- Use custom time input after logging in.

### Main Control Logic
#### setup()
void setup() {
  Serial.begin(115200);  // Initialize serial communication
  pinMode(motorPin, OUTPUT);  // Set motor pin as output
  digitalWrite(motorPin, LOW);  // Ensure motor is off

  // Start Access Point
  WiFi.softAP(ssid, password);

  // Start DNS server
  dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());

  // Setup mDNS for local access
  if (!MDNS.begin(hostname)) {
    Serial.println("Error setting up MDNS responder!");
  }

  // Handle HTTP requests
  server.on("/", handleRoot);           // Serve the main page
  server.on("/control", handleMotorControl);  // Handle motor control
  server.on("/stop", handleMotorStop);  // Stop motor request
  server.begin();  // Start HTTP server
}

- WiFi.softAP(): Starts the ESP8266 as an Access Point (AP).
- dnsServer.start(): Initializes the DNS server to redirect clients to the ESP8266 for captive portal.
- MDNS.begin(): Starts mDNS so that the device can be accessed at hostname.local.
- server.on(): Registers various HTTP request handlers for different routes.

#### HTTP Request Handlersvoid handleRoot() {
  server.send_P(200, "text/html", index_html);  // Serve the main HTML page
}

void handleMotorControl() {
  if (server.hasArg("duration")) {
    int duration = server.arg("duration").toInt();
    controlMotor(duration);  // Start motor for specified duration
    server.send(200, "text/plain", "Motor running for " + String(duration) + " seconds.");
  } else {
    server.send(400, "text/plain", "Invalid request");
  }
}

void handleMotorStop() {
  stopMotor();  // Stop motor immediately
  server.send(200, "text/plain", "Motor stopped immediately.");
}

Shubham, [28-09-2024 12:05 PM]
- handleRoot(): Serves the HTML web interface to the client.
- handleMotorControl(): Starts the motor for the duration specified in the request.
- handleMotorStop(): Immediately stops the motor.

#### Motor Control Functionsvoid controlMotor(int duration) {
  motorStartTime = millis();  // Store the start time
  motorDuration = duration * 1000;  // Convert to milliseconds
  motorRunning = true;
  digitalWrite(motorPin, HIGH);  // Turn the motor ON
}

void stopMotor() {
  digitalWrite(motorPin, LOW);  // Turn the motor OFF
  motorRunning = false;
}

void checkMotorStatus() {
  if (motorRunning && (millis() - motorStartTime >= motorDuration)) {
    stopMotor();  // Stop motor when the duration has elapsed
  }
}

- controlMotor(): Turns the motor ON and stores the duration.
- stopMotor(): Turns the motor OFF.
- checkMotorStatus(): Checks if the motor has run for the specified duration, and stops it if needed.

### Captive Portal Detectionvoid handleCaptivePortal() {
  server.sendHeader("Location", String("http://") + WiFi.softAPIP().toString(), true);
  server.send(302, "text/html", "");  // Redirect clients to the main page
}

bool isCaptivePortalRequest() {
   String hostHeader = server.hostHeader();
  return !(hostHeader.equals(WiFi.softAPIP().toString()));
}

- handleCaptivePortal(): Redirects clients to the ESP8266's IP address if they try to visit a different URL.
- isCaptivePortalRequest(): Checks if the client is trying to access a different site and redirects them.

### Main Loopvoid loop() {
  dnsServer.processNextRequest();  // Handle DNS requests
  server.handleClient();           // Handle HTTP requests
  MDNS.update();                   // Update mDNS responder
  checkMotorStatus();              // Check motor status
}

- dnsServer.processNextRequest(): Processes captive portal DNS requests.
- server.handleClient(): Handles incoming HTTP requests.
- MDNS.update(): Keeps the mDNS service running.
- checkMotorStatus(): Monitors and controls the motor based on the specified duration.

## Features
- Custom time input after login.
- Colorful buttons with animation for better user experience.
- Speech synthesis notifications when the motor starts and stops.
`

This markdown document explains the purpose of each section of your code. You can upload this to GitHub for easy reference by others.
