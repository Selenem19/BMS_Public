# Battery Management System (BMS) for an Electric Racing Motorcycle (120V / 30kW)

 This repository contains the source code, simulation models, hardware design, and validation tools for an advanced BMS. It is custom-built to manage the battery pack (30S2P, 120 V, 40 Ah) of a high-power electric racing motorcycle.

The project features a modular Master-Slave architecture focused on maximizing track performance without compromising safety. Its core features include **inductive active balancing** and **dynamic thermal control**.

##  Repository Structure

- **`/firmware`**: C/C++ source code for the ESP32 microcontroller (ESP-IDF framework). Powered by **FreeRTOS**, it uses a multitasking architecture to keep critical safety routines strictly isolated from communications. It handles state estimation (SoC/SoP), contactor control, thermal management, and the CAN Bus (TWAI) stack.
- **`/hardware`**: Electronic schematics designed in **KiCad**. It includes the slave module topology, featuring an isolated Analog Front-End (AFE) and the power stage for active cell equalization (bidirectional Buck-Boost DC-DC converters).
- **`/Simulation`**: Mathematical models and digital twins built with **MATLAB/Simulink** and **Simscape Electrical**. These are used to validate the closed-loop thermal strategy, the high-frequency balancing feasibility, and the EKF state estimator.
- **`/dashboard`**: A web-based GUI (HTML/JS/CSS) powered by the *Web Serial API*. It acts as a real-time telemetry dashboard and allows controlled fault injection to test the microcontroller's logic (Software-in-the-Loop validation).

## 🛠️ Built With

*   **Embedded Software**: ESP-IDF v5.x, FreeRTOS, C/C++
*   **Hardware Design**: KiCad EDA
*   **Control & Simulation**: MATLAB, Simulink, Simscape Electrical
*   **Validation**: Locally hosted web interface (compatible with Chromium-based browsers)

---
**Author:** Selene Masa Muñoz  
*B.Sc. Thesis in Electronic, Robotics and Mechatronics Engineering*  
*University of Seville (2026)*
