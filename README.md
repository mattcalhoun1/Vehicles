# Vehicles
Software for Arduino-based Robots

This is a collection of Arduino sketches to power Arduino-based vehicles. These vehicles are 
the autonomous robots that perform various tasks (search) with the help of a "Pilot".
The main vehicle is a combination of MecCar and Lookout. Together, they respond
to commands from the Pilot, which is in a separate repo, and runs on a Raspberry Pi or Nvidia Jetson Nano.

**MecCar**

This is my primary vehicle at the moment. It is the host vehicle for a pilot running on a raspberry pi CM4. This sketch runs on an Arduino Due, and handles servos that control dual camera positions, receives data from a Slamtec RPLidar A1M8, and comminicates with the pilot via a serial connection. The sketch also communicates with the Lookout, via another serial connection, for pushing data to the display and monitoring for obstacles near the bumpers.

**Lookout**

This monitors 4 IR obstacle sensors, and also manages the vehicle display. It may perform more functions in the future, but that's it for now. It responds to commands from the MecCar via a software serial interface.

**MegaTank**

This is another vehicle, which is a work in progress, based on the Arduino Mega platform.

**Scanner**

A simple I2C scanner for finding addresses of currently connected devices. I "borrowed" this from various sources that have exactly the same functionality. It's not directly used in any of the vehicles.

### Built With

* [![Arduino][Arduino.cc]][Arduino-url]

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- CONTACT -->
## Contact

[Matt Calhoun](https://linkedin.com/in/matt-calhoun-74572828) - Graduate Student at Maryville University

<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- MARKDOWN LINKS & IMAGES -->
[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?style=for-the-badge&logo=linkedin&colorB=555
[linkedin-url]: https://linkedin.com/in/matt-calhoun-74572828
[product-screenshot]: images/screenshot.png
[Arduino.cc]: http://arduino.cc/en/uploads/Trademark/ArduinoCommunityLogo.png
[Arduino-url]: https://www.arduino.cc/
