#include <ESP8266WiFi.h>       // Wi-Fi functionality
#include <ESP8266WebServer.h>  // HTTP server functionality
#include <ESP8266mDNS.h>       // mDNS functionality
#include <DNSServer.h>         // DNS server for captive portal
#include <Arduino.h>

#pragma region Define
#define LED1 4   // GPIO pin for LED 1
#define LED2 0   // GPIO pin for LED 2
#define LED3 2   // GPIO pin for LED 3
#define LED4 14  // GPIO pin for LED 4
#define LED5 12  // GPIO pin for LED 5
#define LED6 13  // GPIO pin for LED 6
const int ledPins[] = { LED1, LED2, LED3, LED4, LED5, LED6 };  // Array of LED pins
const int numLEDs = sizeof(ledPins) / sizeof(ledPins[0]);
int delayTime = 200;  // Time delay between LED changes (in milliseconds)
const int motorPin = 5;       // Motor control pin
ESP8266WebServer server(80);  // Web server on port 80
DNSServer dnsServer;          // DNS server for captive portal
const byte DNS_PORT = 53;     // DNS port number
bool chaserDirection = true;
int currentLED = 0;
int currentPattern = 0;  // To track which chaser pattern is active
const unsigned long patternInterval = 10000;  // Change pattern every 10 seconds
unsigned long flashInterval = 100;  // Time for flashing all LEDs
unsigned long lastChaserTime = 0;  // Timer for non-blocking chaser
unsigned long lastFlashTime = 0;   // Timer for flashing LEDs
bool flashState = false;
unsigned long patternChangeTime = 0;  // Timer for changing patterns
unsigned long outsideToInsideTimer = 0;  // Timer for outside-to-inside pattern
const char* ssid = "Er Shubham Motor Control";  // Wi-Fi SSID
const char* password = "123454321";             // Wi-Fi password
const char* hostname = "ErShubham";             // mDNS hostname (e.g., shubham.local)
#pragma endregion 

#pragma region HTML
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Water Tank Control</title>
    <style>
        body {
            font-family: Arial, sans-serif;
        }
        #control-panel {
            text-align: center;
            margin: 20px;
        }
        button {
            margin: 5px;
            padding: 10px 20px;
            font-size: 16px;
        }
        @keyframes colorChange {
    0% { color: rgb(233, 5, 5); }
    25% { color: rgb(0, 128, 0); } /* Green */
    50% { color: rgb(0, 0, 255); } /* Blue */
    75% { color: rgb(255, 165, 0); } /* Orange */
    100% { color: rgb(233, 5, 5); } /* Red */
}
#timer-display {
    margin-top: 20px;
    font-family:inherit;
    animation: colorChange 4s infinite;
    font-size: xx-large;
    font-weight: bolder;
}
        #login-section {
            margin-top: 40px;
        }
        #custom-time {
            display: none;
            margin-top: 20px;
        }
        input {
            padding: 5px;
            margin: 5px;
        }
        .stop-btn {
            position: fixed;
            top: 30%;
            right: -12%;
            transform: translateY(-50%) rotate(-90deg);
            background-color: rgb(245, 59, 59);
            color: white;
            padding: 10px 20px;
            font-size: 16px;
            cursor: pointer;
            border: dotted;
            border-radius: 17px;
            box-shadow: -17px -25px -16px rgba(141, 229, 0, 0.7);
        }
            .stop-btn:hover {
                background-color: rgb(249, 5, 5);
            }
            .mainbtn {
                border: none;
                padding: 0.8em 2.5em;
                outline: none;
                color: white;
                font-style: 1.2em;
                position: relative;
                z-index: 1;
                cursor: pointer;
                background: none;
                text-shadow: 3px 3px 10px rgba(0, 0, 0, 0.45);
            }
            .mainbtn:before,
            .mainbtn:after {
                position: absolute;
                top: 50%;
                left: 50%;
                border-radius: 10em;
                -webkit-transform: translateX(-50%) translateY(-50%);
                transform: translateX(-50%) translateY(-50%);
                width: 105%;
                height: 105%;
                content: "";
                z-index: -2;
                background-size: 400% 400%;
                background: linear-gradient(60deg,
                        #f79533,
                        #f37055,
                        #ef4e7b,
                        #a166ab,
                        #5073b8,
                        #1098ad,
                        #07b39b,
                        #6fba82);
            }
            .mainbtn:before {
                -webkit-filter: blur(7px);
                filter: blur(7px);
                -webkit-transition: all 0.25s ease;
                transition: all 0.25s ease;
                -webkit-animation: pulse 10s infinite ease;
                animation: pulse 10s infinite ease;
            }
            .mainbtn:after {
                -webkit-filter: blur(0.3px);
                filter: blur(0.3px);
            }
            .mainbtn:hover:before {
                width: 115%;
                height: 115%;
            }
    </style>
</head>
<body>
    <button class="stop-btn" onclick="stopMotor()">STOP MOTOR</button>
    <div id="control-panel">
        <h1>Water Tank Motor Control</h1>
        <button class="mainbtn" id="btn-1sec" onclick="startTimer(1)">Run for 1 minutes</button>
        <button class="mainbtn" id="btn-5min" onclick="startTimer(5)">Run for 5 minutes</button>
        <button class="mainbtn" id="btn-10min" onclick="startTimer(10)">Run for 10 minutes</button>
        <button class="mainbtn" id="btn-20min" onclick="startTimer(20)">Run for 20 minutes</button>
        <button class="mainbtn" id="btn-30min" onclick="startTimer(30)">Run for 30 minutes</button>
        <button class="mainbtn" id="btn-45min" onclick="startTimer(45)">Run for 45 minutes</button>
        <button class="mainbtn" id="btn-5sec" onclick="startTimer(0.0833)">Run for 5 seconds</button>
        <button class="mainbtn" id="btn-10sec" onclick="startTimer(0.1667)">Run for 10 seconds</button>
        <div id="timer-display"></div>
    </div>
    <div id="login-section">
        <h2>Authorized Access Only</h2>
        <form id="login-form" onsubmit="return handleLogin()">
            <input type="password" id="password" placeholder="Code" required>
            <button class="mainbtn" type="submit">Submit</button>
        </form>
        <div id="custom-time" style="display: none;"> <!-- Hide initially -->
            <label for="customTime">Enter time (minutes):</label>
            <input type="number" id="customTime" min="1" max="60">
            <button class="mainbtn" onclick="startCustomTimer()">Run Custom Time</button>
        </div>
    <script>
          let countdown;
        let loggedIn = false;
        function startTimer(minutes) {
            let xhr = new XMLHttpRequest();
            xhr.open("GET", "/control?duration=" + (minutes * 60), true);
            xhr.send();
              // Trigger text-to-speech announcement
            announceMotorStart(minutes);
            let totalSeconds = Math.floor(minutes * 60);
            document.getElementById("timer-display").innerHTML = "Motor Running...🚰";
            clearInterval(countdown); // Clear any previous timer
            countdown = setInterval(function () {
                if (totalSeconds <= 0) {
                    clearInterval(countdown);
                    let msg = new SpeechSynthesisUtterance(`Motor stopped after ${minutes} minutes .!`);
            window.speechSynthesis.speak(msg);
                    document.getElementById("timer-display").innerHTML = "Motor Stopped.";
                } else {
                    totalSeconds--;
                    let minutesLeft = Math.floor(totalSeconds / 60);
                    let secondsLeft = totalSeconds % 60;
                    document.getElementById("timer-display").innerHTML = `${minutesLeft}:${secondsLeft < 10 ? '0' + secondsLeft : secondsLeft}`;
                }
            }, 1000);
        }
        function announceMotorStart(minutes) {
            let msg = new SpeechSynthesisUtterance(`Motor started for ${minutes} minutes`);
            window.speechSynthesis.speak(msg);
        }
        function stopMotor() {
            let xhr = new XMLHttpRequest();
            xhr.open("GET", "/stop", true);
            xhr.send();
            clearInterval(countdown); // Stop countdown immediately
            document.getElementById("timer-display").innerHTML = "Motor Stopped Immediately !";
            let msg = new SpeechSynthesisUtterance(`Motor stopped immediately!`);
            window.speechSynthesis.speak(msg);
        }
        // Custom time handling
        function startCustomTimer() {
            const customTime = document.getElementById("customTime").value;
            if (customTime && customTime > 0) {
                startTimer(customTime); // Call the startTimer function with custom time (in minutes)
            } else {
                alert("Please enter a valid time (in minutes).");
            }
        }
        function handleLogin() {
            const password = document.getElementById("password").value;
            if (password === "4888") {
                loggedIn = true;
                document.getElementById('custom-time').style.display = "block"; // Show custom time input
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
#pragma endregion 

#pragma region Cheser
// Pattern 1: Left to right (normal chaser)
void chaserPattern1() {
  if (chaserDirection) {
    currentLED++;
    if (currentLED >= numLEDs) {
      currentLED = 0;
    }
  } else {
    currentLED--;
    if (currentLED < 0) {
      currentLED = numLEDs - 1;
    }
  }
}
// Pattern 2: Back and forth
void chaserPattern2() {
  currentLED += (chaserDirection ? 1 : -1);
  if (currentLED >= numLEDs || currentLED < 0) {
    chaserDirection = !chaserDirection;
    currentLED += (chaserDirection ? 1 : -1);
  }
}
// Pattern 3: Flash all LEDs (non-blocking)
void chaserPattern3() {
  if (millis() - lastFlashTime >= flashInterval) {
    lastFlashTime = millis();
    flashState = !flashState;  // Toggle flash state
    for (int i = 0; i < numLEDs; i++) {
      digitalWrite(ledPins[i], flashState ? HIGH : LOW);
    }
  }
}
//Pattern 4: Outside to inside (non-blocking)
void chaserPattern4() {
  static int leftIndex = numLEDs / 2 - 1;
  static int rightIndex = numLEDs / 2;
  static int stage = 0;  // Keeps track of the current stage in the pattern
  if (millis() - outsideToInsideTimer >= delayTime) {
    outsideToInsideTimer = millis();  // Update the last timer
    // Turn off all LEDs
    for (int i = 0; i < numLEDs; i++) {
      digitalWrite(ledPins[i], LOW);
    }
    // Light up LEDs from outside to inside
    if (stage <= leftIndex) {
      digitalWrite(ledPins[leftIndex - stage], HIGH);
      digitalWrite(ledPins[rightIndex + stage], HIGH);
      stage++;
    } else {
      stage = 0;  // Reset stage for next cycle
    }
  }
}
// Pattern: LEDs light up from inside to outside (non-blocking)
void chaserPattern5() {
  static int leftIndex = numLEDs / 2 - 1;  // Start from the middle left
  static int rightIndex = numLEDs / 2;     // Start from the middle right
  static int stage = 0;                    // Stage keeps track of current step in the pattern
  unsigned long insideToOutsideTimer = 0;  // Timer for non-blocking control
  if (millis() - insideToOutsideTimer >= delayTime) {
    insideToOutsideTimer = millis();  // Update the timer
    // Turn off all LEDs before lighting the next pair
    for (int i = 0; i < numLEDs; i++) {
      digitalWrite(ledPins[i], LOW);
    }
    // Light up LEDs from inside to outside
    if (stage <= leftIndex) {
      digitalWrite(ledPins[leftIndex - stage], HIGH);   // Light up from the center-left
      digitalWrite(ledPins[rightIndex + stage], HIGH);  // Light up from the center-right
      stage++;  // Move to the next stage (next outer pair of LEDs)
    } else {
      stage = 0;  // Reset the stage when the cycle completes
    }
  }
}
// Run the current chaser pattern
void runCurrentChaser() {
  if (millis() - lastChaserTime >= delayTime) {
    lastChaserTime = millis();
    // Turn off all LEDs (except in flash pattern)
    if (currentPattern != 2) {
      for (int i = 0; i < numLEDs; i++) {
        digitalWrite(ledPins[i], LOW);
      }
    }
    // Select and run the active pattern
    switch (currentPattern) {
      case 0:
        chaserPattern1();  // Normal left-to-right
        break;
      case 1:
        chaserPattern2();  // Back and forth
        break;
         case 2:
        chaserPattern3();  // Back and forth
        break;
         case 3:
        chaserPattern4();  // Back and forth
        break;
      case 4:
        chaserPattern5();  // Flash all
        return;            // Avoid updating currentLED for this pattern
    }
    // Turn on the current LED (for non-flashing patterns)
    digitalWrite(ledPins[currentLED], HIGH);
  }
  // Change pattern every 10 seconds
  if (millis() - patternChangeTime >= patternInterval) {
    patternChangeTime = millis();
    currentPattern++;
    if (currentPattern > 2) {
      currentPattern = 0;  // Reset to first pattern
    }
  }
}
#pragma endregion

#pragma region  Main Function 
void handleRoot() {
  // Serve the main page
  server.send_P(200, "text/html", index_html);  // Serve the HTML page
}
void handleCaptivePortal() {
  // Redirect to the main page for captive portal clients
  server.sendHeader("Location", String("http://") + WiFi.softAPIP().toString(), true);
  server.send(302, "text/html", "");  // Redirect client
}
bool isCaptivePortalRequest() {
  String hostHeader = server.hostHeader();
  return !(hostHeader.equals(WiFi.softAPIP().toString()));
}
unsigned long motorStartTime = 0;
int motorDuration = 0;
bool motorRunning = false;
bool sosPressed = false;
void controlMotor(int duration) {
  motorStartTime = millis();
  motorDuration = duration * 1000;  // Convert to milliseconds
  motorRunning = true;
  sosPressed = false;
  digitalWrite(motorPin, HIGH);  // Turn motor ON
}
void stopMotor() {
  digitalWrite(motorPin, LOW);  // Turn motor OFF immediately
  motorRunning = false;
  sosPressed = true;
}
void checkMotorStatus() {
  if (motorRunning && (millis() - motorStartTime >= motorDuration)) 
  {
    stopMotor();  // Stop motor when the duration is over
   ;
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
void handleMotorStop() {
  stopMotor();  // Stop motor immediately
  server.send(200, "text/plain", "Motor stopped immediately.");
   ;
}
#pragma endregion Main Function 

#pragma region setup
void setup() {
  Serial.begin(115200);         // Initialize Serial
  pinMode(motorPin, OUTPUT);    // Set motor pin as output
  digitalWrite(motorPin, LOW);  // Ensure motor is off
  // Start Access Point
  WiFi.softAP(ssid, password);
  Serial.println("Access Point started. IP address: ");
  Serial.println(WiFi.softAPIP());  // Print IP address
  // Start DNS server to catch all DNS requests and redirect to ESP8266
  dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
  // Start mDNS for hostname.local
  if (!MDNS.begin(hostname)) {
    Serial.println("Error setting up MDNS responder!");
  } else {
    Serial.println("mDNS responder started. Access at http://" + String(hostname) + ".local");
  }
  // Handle captive portal detection for Android, iOS, etc.
  server.on("/generate_204", handleCaptivePortal);         // For Android
  server.on("/hotspot-detect.html", handleCaptivePortal);  // For iOS
  server.on("/fwlink", handleCaptivePortal);               // For Windows (older versions)
  // Handle root requests and motor control
  server.on("/", handleRoot);                 // Handle root URL requests
  server.on("/control", handleMotorControl);  // Handle motor control requests
  server.on("/stop", handleMotorStop);        // Handle motor stop requests (SOS)
  // Start HTTP server
  server.begin();
  Serial.println("HTTP server started");
  for (int i = 0; i < numLEDs; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);  // Ensure all LEDs are off initially
  }
}
#pragma endregion 

#pragma region loop
void loop() {
  dnsServer.processNextRequest();  // Process captive portal DNS requests
  server.handleClient();           // Handle HTTP requerunChaser()sts
  MDNS.update();                   // Keep mDNS running
  checkMotorStatus();              // Check motor status and turn off if needed
  if (isCaptivePortalRequest()) {
    handleCaptivePortal();
  }
// Run chaser only if motor is not running
if (!motorRunning) {
    runCurrentChaser();  // Non-blocking LED chaser
  }
  // Immediately stop motor if SOS button was pressed
  if (sosPressed && motorRunning) {
    stopMotor();
     ;
  }
}
#pragma endregion 
