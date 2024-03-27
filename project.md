# Project Ideas

## Fall Sensor:
- **Description**: A device that can detect when a person has fallen and alert someone.

### Process of working:
- Tracks the person's movement using a gyroscope and accelerometer.
- Checks changes in velocity and acceleration.
- If a fall is detected, sends a message to a phone
- Goes to Low-power mode when the velocity isn't changing, have an LED to show

### Hardware Reqs:
- Gyro/Accelerometer: MPU6050 (I2C)
- Plastic Casing/ Protection (3D Printed)
- Bluetooth Communication: HC-05 (UART)
- Microcontroller: MSP-EXP430FR2433 https://www.mouser.com/ProductDetail/Texas-Instruments/MSP-EXP430FR2433?qs=j%252B1pi9TdxUbUYSWQHmscvw%3D%3D