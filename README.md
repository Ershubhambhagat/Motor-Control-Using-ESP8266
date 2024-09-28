

# ESP8266 Water Tank Motor Control

This project controls a water tank motor via an ESP8266 using a web interface. It provides a user-friendly UI for controlling the motor based on predefined durations, and allows manual stopping of the motor. It also includes captive portal functionality to make it easier for users to connect to the device's Wi-Fi network.


# ESP8266-Based Water Tank Motor Control System

This project enables wireless control of a water tank motor using the ESP8266 microcontroller. With a clean, intuitive web interface accessible via any device, users can set the motor to run for predefined periods, or stop it instantly. The system creates its own Wi-Fi network, allowing easy connection without needing access to existing networks. It also features a secure login mechanism for custom motor operation.

## Overview

The system is designed for simplicity and reliability. Once connected to the ESP8266's Wi-Fi network, users can access the control panel from any web browser. The panel provides buttons to start the motor for durations ranging from seconds to several minutes, and a dedicated "STOP" button for emergency halts. The system also features a login section, allowing only authorized users to set a custom time duration for motor operation.

## Key Features

- **Standalone Wi-Fi Network**: The ESP8266 creates its own access point, ensuring that users can connect directly without needing access to external routers or networks. This makes the system highly portable and usable in remote locations.
  
- **Responsive Web Interface**: The control interface is optimized for both desktop and mobile devices, making it easy to operate the motor from anywhere within the Wi-Fi range. The interface features large, clearly labeled buttons for convenience.

- **Predefined Time Options**: Users can choose from several preset options to run the motor for durations such as 1, 5, 10, or 30 minutes. This ensures quick and easy control for common usage scenarios.

- **Speech Notifications**: To enhance user feedback, the system provides real-time voice announcements through the browser when the motor starts and stops. This ensures the user is always informed of the motor's status without needing to constantly check the interface.

- **Emergency Stop Button**: For immediate control, a "STOP MOTOR" button is prominently featured on the interface, allowing users to halt the motor at any time, regardless of the current timer setting.

- **Custom Timer (Login Protected)**: For advanced users, the interface includes a custom timer feature, which becomes available after logging in with a secure password. This allows for flexible operation, giving users full control over how long the motor should run.

## User Experience

Upon connecting to the ESP8266’s network, users are automatically redirected to the control page. The page is visually appealing, with animated buttons that change color for a dynamic user experience. The control panel is centered on the screen, with all available options clearly visible. A timer countdown is displayed once the motor is running, providing users with precise feedback on how much time remains.

An emergency "STOP" button is always accessible, ensuring that users can immediately halt the motor if necessary. Speech notifications keep users informed without requiring them to look at the screen.

For additional security, the custom timer feature is protected by a password. Once authenticated, users can input a custom time in minutes, allowing the motor to run for any duration between 1 and 60 minutes.

## Security

To ensure secure operation, the custom timer functionality is protected by a login mechanism. Users must enter a predefined password to gain access to the custom timer settings, ensuring that only authorized personnel can make changes beyond the preset options.

## Captive Portal and mDNS Support

The system features captive portal functionality, which simplifies the process of connecting to the device's network. Once connected, users are automatically redirected to the control interface without needing to manually enter an IP address. Additionally, mDNS support allows users to access the system via a friendly local hostname (e.g., `shubham.local`), further streamlining the connection process.

## Technical Summary

This project is built around the ESP8266 microcontroller, leveraging its built-in Wi-Fi capabilities to create a standalone access point and web server. The device serves an interactive web page, written in HTML and JavaScript, which allows users to control the motor. The system handles HTTP requests to start or stop the motor, and provides real-time feedback to users through both visual and auditory cues. With the use of speech synthesis, the system enhances user interaction by providing vocal status updates on motor operation.