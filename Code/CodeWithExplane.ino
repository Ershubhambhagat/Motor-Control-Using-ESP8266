
// Include necessary libraries for Wi-Fi, web server, and DNS functionality
#include <ESP8266WiFi.h>       // Handles Wi-Fi functionality
#include <ESP8266WebServer.h>  // Handles HTTP server functionality
#include <DNSServer.h>         // Handles DNS requests

// Define the GPIO pin connected to the motor
const int motorPin = D1;  // Pin D1 (GPIO5) used for motor control

// Create an instance of the web server on port 80
ESP8266WebServer server(80);

// Create an instance of the DNS server
DNSServer dnsServer;

// Wi-Fi credentials for the Access Point (AP)
const char* ssid = "ESP8266_WaterControl";  // SSID of the Wi-Fi network created by the ESP8266
const char* password = "12345678";          // Password for the Wi-Fi network

// HTML page to be served by the ESP8266 (stored in program memory)
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Water Tank Control</title>
    <style>
        body { font-family: Arial, sans-serif; }
        #control-panel { text-align: center; margin: 20px; }
        button { margin: 5px; padding: 10px 20px; font-size: 16px; }
        #timer-display { font-size: 24px; margin-top: 20px; color: red; }
        #login-section { margin-top: 40px; }
        #custom-time { display: none; margin-top: 20px; }
        input { padding: 5px; margin: 5px; }
    </style>
</head>
<body>
    <div id="control-panel">
        <h1>Water Tank Motor Control</h1>
        <button id="btn-5min" onclick="startTimer(5)">Run for 5 minutes</button>
        <button id="btn-10min" onclick="startTimer(10)">Run for 10 minutes</button>
        <button id="btn-15min" onclick="startTimer(15)">Run for 15 minutes</button>
        <button id="btn-20min" onclick="startTimer(20)">Run for 20 minutes</button>

        <div id="timer-display"></div>
    </div>

    <!-- Login for Authorized Users -->
    <div id="login-section">
        <h2>Authorized Access</h2>
        <form id="login-form" onsubmit="return handleLogin()">
            <input type="text" id="username" placeholder="Username" required>
            <input type="password" id="password" placeholder="Password" required>
            <button type="submit">Login</button>
        </form>
        <div id="custom-time">
            <label for="customTime">Enter time (minutes):</label>
            <input type="number" id="customTime" min="1" max="30">
            <button onclick="startCustomTimer()">Run Custom Time</button>
        </div>
    </div>

    <script>
        let countdown;
        let loggedIn = false;

        function startTimer(minutes) {
            let totalSeconds = minutes * 60;
            disableButtons();

            countdown = setInterval(function() {
                if (totalSeconds <= 0) {
                    clearInterval(countdown);
                    enableButtons();
                    document.getElementById("timer-display").innerHTML = "Motor stopped.";
                } else {
                    totalSeconds--;
                    let minutesLeft = Math.floor(totalSeconds / 60);
                    let secondsLeft = totalSeconds % 60;
                    document.getElementById("timer-display").innerHTML = `${minutesLeft}:${secondsLeft < 10 ? '0' + secondsLeft : secondsLeft}`;
                }
            }, 1000);
        }

        function startCustomTimer() {
            let customMinutes = document.getElementById('customTime').value;
            if (customMinutes > 30) {
                customMinutes = 30;
            }
            startTimer(customMinutes);
        }

        function disableButtons() {
            document.querySelectorAll("button").forEach(button => {
                button.disabled = true;
            });
        }

        function enableButtons() {
            document.querySelectorAll("button").forEach(button => {
                button.disabled = false;
            });
        }

        function handleLogin() {
            const username = document.getElementById("username").value;
            const password = document.getElementById("password").value;

            // Simple username/password check
            if (username === "admin" && password === "password") {
                loggedIn = true;
                document.getElementById('custom-time').style.display = "block";
                alert("Login successful!");
            } else {
                alert("Invalid credentials");
            }

            return false; // Prevent form submission
        }
    </script>
</body>
</html>
)rawliteral";

// Function to handle HTTP requests to the root path "/"
void handleRoot() {
  server.send_P(200, "text/html", index_html);
}

// Function to control the motor (turn it on for the specified duration)
void controlMotor(int duration) {
  digitalWrite(motorPin, HIGH);  // Turn motor ON
  delay(duration * 1000);        // Keep motor ON for the duration (in milliseconds)
  digitalWrite(motorPin, LOW);   // Turn motor OFF
}

// Setup function runs once when the ESP8266 starts
void setup() {
  Serial.begin(115200);  // Initialize serial communication for debugging

  // Set the motor pin as an output and ensure the motor is off initially
  pinMode(motorPin, OUTPUT);
  digitalWrite(motorPin, LOW);

  // Start the ESP8266 as an Access Point
  WiFi.softAP(ssid, password);
  Serial.println();
  Serial.print("Access Point started. IP address: ");
  Serial.println(WiFi.softAPIP()); // Print the IP address of the Access Point to the Serial Monitor

  // Set up the DNS server to respond to all domain name requests with the ESP8266 IP address
  dnsServer.start(53, "*", WiFi.softAPIP());

  // Handle HTTP requests to the root path "/"
  server.on("/", handleRoot);

  // Start the HTTP server
  server.begin();
}

// Loop function runs repeatedly after setup()
void loop() {
  server.handleClient();        // Handle incoming client requests to the HTTP server
  dnsServer.processNextRequest(); // Handle DNS requests
}
