# Water Tank Motor Control with ESP8266

This project uses the ESP8266 microcontroller to create a Wi-Fi network and host a web page for controlling a water tank motor. The ESP8266 operates as a standalone access point, allowing you to connect to its network with your phone and access the control interface.

## Features

- **Wi-Fi Access Point (AP)**: The ESP8266 creates its own Wi-Fi network named `ESP8266_WaterControl`, which you can connect to from your phone.
- **Web Interface**: The ESP8266 serves a web page that allows you to control the water tank motor. The interface includes buttons to run the motor for predefined durations (5, 10, 15, or 20 minutes) and a login system for custom durations.
- **Countdown Timer**: A visual countdown timer on the web page displays the remaining time while the motor is running.
- **Login System**: A simple login form is included to allow authorized users to input custom durations for running the motor. Users can access this feature by entering a predefined username and password.
- **Custom DNS**: The project uses a local DNS server, allowing you to access the control page via a custom domain name like `watercontrol.local` instead of using the IP address.

## How It Works

1. **Setup**: Power up the ESP8266. It will start its own Wi-Fi network with the SSID `ESP8266_WaterControl`. Connect your phone to this network.
2. **Access the Web Page**: Open a browser on your phone and navigate to `watercontrol.local` to access the control page.
3. **Control the Motor**: Use the buttons on the web page to start the motor for 5, 10, 15, or 20 minutes. The timer will display the remaining time, and the buttons will be disabled while the motor is running.
4. **Authorized Access**: For running the motor for more than 20 minutes, log in using the provided username and password to access a field where you can enter custom durations.

## Installation

1. **Connect the ESP8266**: Ensure your ESP8266 is properly connected to the motor and power supply.
2. **Upload Code**: Use the Arduino IDE to upload the provided code to your ESP8266. This code configures the ESP8266 as a Wi-Fi access point, serves the web page, and controls the motor.
3. **Connect and Control**: Connect your phone to the ESP8266's Wi-Fi network and open the control page in a browser to start using the interface.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Thanks to the Arduino and ESP8266 communities for their support and documentation.

Feel free to modify and use this project as needed. If you have any questions or issues, please open an issue on this repository.

>http://192.168.4.1/control?duration=5