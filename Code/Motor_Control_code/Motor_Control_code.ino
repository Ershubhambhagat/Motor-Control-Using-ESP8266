#include <ESP8266WiFi.h>       // Wi-Fi functionality
#include <ESP8266WebServer.h>  // HTTP server functionality

const int motorPin = 5;  // Motor control pin

ESP8266WebServer server(80);  // Web server on port 80

const char* ssid = "ESP8266_WaterControl";  // Wi-Fi SSID
const char* password = "12345678";          // Wi-Fi password

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
        <button id="btn-5sec" onclick="startTimer(0.0833)">Run for 5 seconds</button>
        <button id="btn-10sec" onclick="startTimer(0.1667)">Run for 10 seconds</button>
        <button id="btn-5min" onclick="startTimer(5)">Run for 5 minutes</button>
        <button id="btn-10min" onclick="startTimer(10)">Run for 10 minutes</button>
        <button id="btn-15min" onclick="startTimer(15)">Run for 15 minutes</button>
        <button id="btn-20min" onclick="startTimer(20)">Run for 20 minutes</button>

        <div id="timer-display"></div>
    </div>

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
            let xhr = new XMLHttpRequest();
            xhr.open("GET", "/control?duration=" + (minutes * 60), true);
            xhr.send();
            let totalSeconds = Math.floor(minutes * 60);
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
            if (username === "admin" && password === "password") {
                loggedIn = true;
                document.getElementById('custom-time').style.display = "block";
                alert("Login successful!");
            } else {
                alert("Invalid credentials");
            }
            return false;  // Prevent form submission
        }
    </script>
</body>
</html>
)rawliteral";

unsigned long motorStartTime = 0;
int motorDuration = 0;
bool motorRunning = false;

void handleRoot() {
  server.send_P(200, "text/html", index_html);  // Serve the HTML page
}

void controlMotor(int duration) {
  motorStartTime = millis();
  motorDuration = duration * 1000;  // Convert to milliseconds
  motorRunning = true;
  digitalWrite(motorPin, HIGH);  // Turn motor ON
}

void checkMotorStatus() {
  if (motorRunning && (millis() - motorStartTime >= motorDuration)) {
    digitalWrite(motorPin, LOW);  // Turn motor OFF
    motorRunning = false;
  }
}

void handleMotorControl() {
  if (server.hasArg("duration")) {
    int duration = server.arg("duration").toInt();
    controlMotor(duration);
    server.send(200, "text/plain", "Motor running for " + String(duration) + " seconds.");
  } else {
    server.send(400, "text/plain", "Invalid request");
  }
}

void setup() {
  Serial.begin(115200);  // Initialize Serial
  pinMode(motorPin, OUTPUT);  // Set motor pin as output
  digitalWrite(motorPin, LOW);  // Ensure motor is off

  WiFi.softAP(ssid, password);  // Start Access Point
  Serial.println("Access Point started. IP address: ");
  Serial.println(WiFi.softAPIP());  // Print IP address

  server.on("/", handleRoot);  // Handle root URL requests
  server.on("/control", handleMotorControl);  // Handle motor control requests
  server.begin();  // Start HTTP server
}

void loop() {
  server.handleClient();  // Handle HTTP requests
  checkMotorStatus();     // Check motor status and turn off if needed
}
