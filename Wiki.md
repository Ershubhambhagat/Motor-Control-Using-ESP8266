
# Setting Up ESP8266 to Control a Motor with Captive Portal

## Objective
This document provides step-by-step instructions on how to configure an ESP8266 to control a motor using a webpage interface. It includes details for setting up a captive portal that automatically displays the control page when a user connects to the ESP8266 Wi-Fi network. The document also covers adding user login for advanced actions and timer controls for motor operation.

## Requirements
- **Hardware**: ESP8266 or NodeMCU, motor, motor driver
- **Software**: Arduino IDE
- **Libraries**:
  - ESP8266WiFi
  - ESP8266WebServer
  - DNSServer
  - Ticker

---

## Step 1: Hardware Setup
1. **Components**:
   - ESP8266 or NodeMCU
   - Motor (connected to a motor driver)
   - Power supply (for motor and ESP8266)
   - Breadboard and jumper wires

2. **Connections**:
   - Connect the motor to the motor driver, and wire the driver to the ESP8266 GPIO pins for control.
   - Power the ESP8266 and the motor appropriately.

---

## Step 2: Setting Up Arduino IDE for ESP8266
1. **Install ESP8266 Libraries**:
   - Go to `File -> Preferences` in the Arduino IDE.
   - Add the following URL to the “Additional Board Manager URLs” field:
     ```
     http://arduino.esp8266.com/stable/package_esp8266com_index.json
     ```
   - Go to `Tools -> Board -> Board Manager` and search for **ESP8266**. Install the package.

2. **Required Libraries**:
   - Install the following libraries using Arduino Library Manager:
     - **ESP8266WiFi**
     - **ESP8266WebServer**
     - **DNSServer**
     - **Ticker**

---

## Step 3: Setting Up ESP8266 as an Access Point (AP)
1. **Configure ESP8266 as AP**:
   - In the `setup()` function, initialize the ESP8266 as an Access Point (AP) to create a local Wi-Fi network.
     ```cpp
     WiFi.softAP("MotorControl-AP", "password123");
     IPAddress IP = WiFi.softAPIP();
     ```

2. **Connect Devices**:
   - Once the ESP8266 is in AP mode, connect a phone or laptop to the network. The IP of the ESP8266 is typically `192.168.4.1`.

---

## Step 4: Setting Up a Web Server to Control the Motor
1. **Create a Web Server**:
   - Use the `ESP8266WebServer` library to create a web server that serves an HTML page with buttons for controlling the motor.

2. **Web Page Layout**:
   - The webpage should have buttons for controlling the motor for different durations (5, 10, 15, 20 minutes), and a hidden input for entering a code to allow extended runtime.
   
   Example HTML:
   ```html
   <button onclick="startMotor(5)">5 Minutes</button>
   <button onclick="startMotor(10)">10 Minutes</button>
   <input type="text" id="authCode" placeholder="Enter code">
   <button onclick="startMotorWithCode()">Run with Code</button>
   ```

3. **Handling Motor Control**:
   - Write functions in the `.ino` file to handle motor control when buttons are clicked.
     ```cpp
     server.on("/startMotor", []() {
         String time = server.arg("time");
         startMotor(time.toInt());
         server.send(200, "text/plain", "Motor started for " + time + " minutes");
     });
     ```

---

## Step 5: Implementing Timers for Motor Control
1. **Use Ticker Library for Timers**:
   - The `Ticker` library can be used to manage countdown timers for the motor.
     ```cpp
     Ticker motorTimer;

     void startMotor(int duration) {
         digitalWrite(motorPin, HIGH);  // Start motor
         motorTimer.once(duration * 60, stopMotor);  // Stop motor after duration
     }

     void stopMotor() {
         digitalWrite(motorPin, LOW);  // Stop motor
     }
     ```

---

## Step 6: Adding User Authentication for Extended Motor Run Time
1. **Create a Simple Login System**:
   - Use a simple input for username and password comparison.
     ```cpp
     String username = "admin";
     String password = "password123";

     server.on("/login", []() {
         if (server.arg("user") == username && server.arg("pass") == password) {
             // Allow extended motor run
         } else {
             server.send(403, "text/plain", "Unauthorized");
         }
     });
     ```

2. **Restrict Motor Duration**:
   - If the user is not logged in, limit the motor run time to 30 minutes. Logged-in users can run the motor for longer durations.

---

## Step 7: Implementing a Countdown Timer and Dynamic Color Change
1. **Dynamic Countdown**:
   - Update the webpage to show a real-time countdown timer and change the background color from green to red as the timer runs.

2. **JavaScript for Countdown**:
   - Example JavaScript:
     ```javascript
     function startCountdown(duration) {
         let timeLeft = duration * 60;
         const interval = setInterval(() => {
             document.getElementById('timer').innerText = `${Math.floor(timeLeft / 60)}:${timeLeft % 60}`;
             if (timeLeft <= 0) {
                 clearInterval(interval);
                 document.body.style.backgroundColor = "red";  // Change to red when complete
             } else {
                 document.body.style.backgroundColor = `rgb(0, ${255 * (timeLeft / (duration * 60))}, 0)`;  // Dynamic color
             }
             timeLeft--;
         }, 1000);
     }
     ```

---

## Step 8: Captive Portal Implementation
1. **DNS Redirection**:
   - Use the `DNSServer` library to redirect all DNS requests to the ESP8266’s IP (`192.168.4.1`).
     ```cpp
     DNSServer dnsServer;

     void setup() {
         dnsServer.start(53, "*", WiFi.softAPIP());  // Redirect all DNS requests to the ESP8266
     }
     ```

---

## Step 9: Testing the System
1. **Connect to Wi-Fi**:
   - Connect to the ESP8266’s Wi-Fi network. The captive portal should automatically open the control page, or you can manually navigate to `192.168.4.1`.

2. **Test Motor Control**:
   - Click the buttons to start the motor and ensure that the countdown and color change work correctly.

3. **Authentication**:
   - Test the login system to confirm that only authorized users can run the motor for more than 30 minutes.



