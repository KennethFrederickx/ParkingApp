# ParkingApp

ParkingApp is an IoT-based project that manages traffic lights and monitors parking availability using ultrasonic sensors and MQTT communication. The project integrates various FIWARE components to provide real-time context information and visualization.

## Features

- **Ultrasonic Sensor Integration**: Uses ESP32 and HC-SR04 sensors to measure distance and monitor parking spaces.
- **Traffic Light Control**: Automatically controls traffic lights based on sensor data to manage parking space availability.
- **MQTT Communication**: Utilizes MQTT protocol for efficient message exchange between sensors, traffic lights, and the central server.
- **OAuth2 Authentication**: Secure token-based authentication using FIWARE Keyrock.
- **Context Management**: Real-time context data handling using FIWARE Orion-LD context broker.
- **Data Persistence and Visualization**: Stores sensor data in CrateDB and provides visual analytics using Grafana.

## Components

### ESP32 with Ultrasonic Sensor:
- `ultrasonicsensor.ino`: Measures distance and publishes data to the MQTT broker.

### ESP32 with Traffic Lights:
- `trafficlights.ino`: Controls LEDs for traffic lights based on messages from the MQTT broker.

### Python Script:
- `script.py`: Manages MQTT communication, processes sensor data, controls traffic lights, and updates context information in Orion-LD.

### Helper Functions:
- `ultraSonicSensorFunctions.py`: Processes ultrasonic sensor data, manages context entities, and handles subscriptions.

### Docker Setup:
- `docker-compose.yml`: Sets up the necessary FIWARE services including Orion-LD, MongoDB, QuantumLeap, CrateDB, Grafana, and Keyrock.

## Getting Started

1. Clone the Repository:
    ```bash
    git clone https://github.com/KennethFrederickxPXL/ParkingApp.git
    cd ParkingApp
    ```
2. Deploy Docker Services:
    ```bash
    docker-compose up -d
    ```
3. Upload Arduino Scripts to their respective ESP32 devices using the Arduino IDE.
4. Run the Python Script:
    ```bash
    python script.py
    ```

## Prerequisites
- Docker and Docker Compose
- Arduino IDE
- Python 3.x
- MQTT Broker (e.g., test.mosquitto.org)

## Future Enhancements
- Implement additional sensor types for more comprehensive parking management.
- Enhance security and scalability of the system.
- Integrate advanced analytics for better decision-making.

## Contributing
Feel free to fork this repository and submit pull requests. For major changes, please open an issue first to discuss what you would like to change.

## License
This project is licensed under the MIT License.
